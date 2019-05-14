#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include <string.h>


typedef unsigned long uint32_t;

#define RESET_MANAGER_BASE 0xFFD05000
#define MPU_RESET_OFFSET   (0x10/4)
#define MAP_SIZE			0X1000


#define TRAMPLOLINE_SIZE 16
static int _boot_cpu(int cpu_id, unsigned int load_addr);

static uint32_t zh(char s[])
{
   int i,m,n;
   uint32_t temp = 0;
   m=strlen(s);//十六进制是按字符串传进来的，所以要获得他的长度
  for(i=0;i<m;i++)
  {
     if(s[i]>='A'&&s[i]<='F')//十六进制还要判断他是不是在A-F或者a-f之间a=10。。
       n=s[i]-'A'+10;
     else if(s[i]>='a'&&s[i]<='f')
       n=s[i]-'a'+10;
     else n=s[i]-'0';
     
     temp=temp*16+n;
   }
   return temp;
}

int main(int argc, char** argv)
{
    //CPU1 start addr=0xFFFF0000 in OCRAM
    //printf("size of int: %d\n",sizeof(int));
    //printf("size of int32: %d\n",sizeof(uint32_t));

    if (argv[1]) 
	_boot_cpu(1,zh(argv[1]));
    else
               _boot_cpu(1,0);
	
    return 0;
}

static void local_delay(unsigned int ticks)
{
   sleep(ticks);
}

static void local_memcpy(char * dest,char*src, unsigned int size)
{
   int i;
   for(i=0;i<size;i++)
   {
      dest[i] = src[i];
   }
}

static int _boot_cpu(int cpu_id, unsigned int load_addr)
{
    extern char altera_trampoline;
    extern char altera_trampoline_jump;
    extern char altera_trampoline_end;	
    unsigned int tramp_size;
    unsigned int tramp_addr = 0x00000000;
    unsigned char mm[TRAMPLOLINE_SIZE];
    int fd;
    unsigned int *cpu1_reset_ptr;
    unsigned int *reset_cmd_ptr;

    fd= open("/dev/mem", O_RDWR|O_SYNC);
    if(fd < 0){
        printf(" failed to open /dev/mem\n");
        return -1;
    }
    cpu1_reset_ptr = (unsigned int *)mmap(NULL,
            MAP_SIZE,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            fd,
            tramp_addr );

    if(cpu1_reset_ptr == MAP_FAILED){
        printf("mmap failed\n");
        return -1;
    }	

    printf("Load address: %08X\n",load_addr);

    if (load_addr) {
        tramp_size = altera_trampoline_end - altera_trampoline;
        if ((load_addr < tramp_size) || (load_addr & 0x3)) {
            return -1;
        }
        //printf("tramp %d\n",tramp_size);
        tramp_size = &altera_trampoline_jump - &altera_trampoline;
                //printf("tramp %d\n",tramp_size);
        local_memcpy((char *)mm,(char *)cpu1_reset_ptr, TRAMPLOLINE_SIZE);
     /*
    	 * Trampoline code is copied to address 0 from where remote core is expected to
    	 * fetch first instruction after reset.If master is using the address 0 then
    	 * this mem copy will screwed the system. It is user responsibility to not
    	 * copy trampoline code in such cases.
    	 *
     */
        local_memcpy((char *)cpu1_reset_ptr, &altera_trampoline, tramp_size);
    	/* Write image address at the word reserved at the trampoline end */
    	*((unsigned int *)((char*)cpu1_reset_ptr + tramp_size)) =  load_addr;
    }

    //flush_cache_all();
    // local_delay(10);
    //fsync(fd);//OK
    msync(cpu1_reset_ptr,TRAMPLOLINE_SIZE,MS_SYNC);
    reset_cmd_ptr = (unsigned int *)mmap(NULL,
            MAP_SIZE,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            fd,
            RESET_MANAGER_BASE );
    if(reset_cmd_ptr == MAP_FAILED)
    {
        printf("cmd mmap failed\n");
    }
    else
    {
        printf("Ready to reset Core1\n");
        *((unsigned int *)(reset_cmd_ptr + MPU_RESET_OFFSET)) |= 0x00000002;
        msync(reset_cmd_ptr + MPU_RESET_OFFSET,4,MS_SYNC);
        local_delay(1);
        printf("Core1 disabled\n");
        *((unsigned int *)(reset_cmd_ptr + MPU_RESET_OFFSET)) &= ~0x00000002;	//de-assert CPU1 reset
        msync(reset_cmd_ptr + MPU_RESET_OFFSET,4,MS_SYNC);
        local_delay(1);
        printf("Core1 reseted\n");
    }


    if(load_addr)
    {
        local_memcpy((char *)cpu1_reset_ptr,(char *)mm, TRAMPLOLINE_SIZE);
        printf("Start Clean up ...\n");
        msync(cpu1_reset_ptr,TRAMPLOLINE_SIZE,MS_SYNC);
        printf("Clean up finished\n");
    }
        
    close(fd);
    return 0;
}


