#include<linux/kernel.h>
#include<linux/uaccess.h>

typedef struct mystruct{
    unsigned char delay;
    unsigned char repeat;
    unsigned char position;
    unsigned char value; // dot에 띄워주자
};

asmlinkage int sys_newcall(int interval, int rep, unsigned char *number){


    struct mystruct input;
    unsigned char tmp[4];    

    int remaining; // copy_from_user가 제대로 되었는가?
    int i; // temporary
    printk("interval: %d, rep: %d, num: %s",interval, rep, number);
    remaining = copy_from_user(tmp, number, 4); // input.value & chu ga ham
    // 주소 넘겨주는거니까 copy to user 사용
    if(remaining){
        printk("copy_from_user_value failed\n");
        return -1;
    }

    input.delay = (unsigned char)interval;
    input.repeat = (unsigned char)rep; // 255를 넘어가지 않으므로 단순히 typecast해도 상관없음

    for(i=0;i<4;i++){ // 4자리 수 받아오기
        if(tmp[i]!= '0'){
            input.position = i;
            input.value = tmp[i];
        }
    }
    
    return *(int *)(&input);

}



