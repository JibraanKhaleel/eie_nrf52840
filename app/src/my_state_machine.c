/**
 * @file my_state_machine.c
 */

#include <zephyr/smf.h>
#include <math.h>
#include "LED.h"
#include "BTN.h"
#include "my_state_machine.h"
#include <string.h>



int state_value;
unsigned char temp_value = 0;
int btncode1 = 0;  
int btncode0 = 0;
char main_string[50] = ""; 
uint32_t both_hold_ms = 0;

int standby_check(void){
    if(BTN_is_pressed(BTN0) && BTN_is_pressed(BTN1)) {
        if(both_hold_ms < 3000) {
            both_hold_ms++;
        } else {
            return 1;
        }
    } else {
        both_hold_ms = 0;
    }
    return 0;
}

//prototypes

    //S0 prototypes
static void s0_enter(void* o);
static void s0_exit(void* o);
static enum smf_state_result s0_run(void* o);


    //S1 prototypes
static void s1_enter(void* o);
static void s1_exit(void* o);
static enum smf_state_result s1_run(void* o);
    

    //S2 prototypes
static void s2_enter(void* o);
static void s2_exit(void* o);
static enum smf_state_result s2_run(void* o);

    //S3 prototypes
static void s3_enter(void* o);
static void s3_exit(void* o);
static enum smf_state_result s3_run(void* o);

//typedefs



enum led_state_machine_states{
    s0,
    s1,
    s2,
    s3,
};
typedef struct {
    //context var used to track state machine state, must be first
    struct smf_ctx ctx;
    uint16_t count;
} led_state_object_t; //defining led_state_object type

//Local vars (commented out unnecessary stuff from Lesson)

static const struct smf_state led_states[] = {
    [s0] = SMF_CREATE_STATE(s0_enter, s0_run, s0_exit, NULL, NULL),
    [s1] = SMF_CREATE_STATE(s1_enter, s1_run, s1_exit, NULL, NULL),
    [s2] = SMF_CREATE_STATE(s2_enter, s2_run, s2_exit, NULL, NULL),
    [s3] = SMF_CREATE_STATE(s3_enter, s3_run, s3_exit, NULL, NULL),
};

static led_state_object_t led_state_object;

void state_machine_init() {
    led_state_object.count = 0; //accesses the "count" within the struct named led_state_object of type led_state_object_t, makes it =0
    smf_set_initial(SMF_CTX(&led_state_object), &led_states[s0]); //Initializes state machine in State 0
    state_value = 0;
}

int state_machine_run() {
    return smf_run_state(SMF_CTX(&led_state_object));
}


//s0 functions
static void s0_enter(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
    state_value = 0;
}

static enum smf_state_result s0_run(void* o) {
    static uint16_t flash_count = 0;
    static int flash_active = 0;
    static uint16_t pressed_time;

    if(standby_check()) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s3]);
        return SMF_EVENT_HANDLED;
    }

    if(led_state_object.count >= 500) {             //  1/2 s toggle to blink LED 3 at 1 hz
        LED_toggle(LED3);
        if(btncode1 == 1)
            LED_toggle(LED1);                      
        else if(btncode0 == 1)                             
            LED_toggle(LED0);                       
        led_state_object.count = 0;
    } else {
        led_state_object.count++;
    }
    
    if(BTN_check_clear_pressed(BTN0) && !BTN_check_clear_pressed(BTN1)) {  
        k_msleep(50);
        if(!BTN_is_pressed(BTN1)) {
            LED_set(LED0, LED_OFF);
            LED_set(LED1, LED_OFF);
            LED_set(LED3, LED_OFF);
            LED_set(LED2, LED_OFF);
            btncode0 = 1;                             //  set input code to 0 to indicate btn 0 pressed         
            btncode1 = 0;
            temp_value <<= 1;
            temp_value |= 0;
        }
    }

    if(BTN_check_clear_pressed(BTN1) && !BTN_check_clear_pressed(BTN0)) {
        k_msleep(50);
        if(!BTN_is_pressed(BTN0)) {
            LED_set(LED0, LED_OFF);
            LED_set(LED1, LED_OFF);
            LED_set(LED2, LED_OFF);
            LED_set(LED3, LED_OFF);
            btncode0 = 0;
            btncode1 = 1;
            temp_value <<= 1;
            temp_value |= 1;
        }
    }

    if(BTN_check_clear_pressed(BTN2)) {
        btncode0 = 0;
        btncode1 = 0;
        temp_value = 0;
        flash_count = 0;
        flash_active = 1;
    }

    if(flash_active) {
        if(flash_count < 250){
            LED_set(LED0, LED_ON);
            LED_set(LED1, LED_ON);
            flash_count++;
        }
        else {
            LED_set(LED0, LED_OFF);
            LED_set(LED1, LED_OFF);
            flash_active = 0;
        }
    }

    if(BTN_check_clear_pressed(BTN3)) {
        size_t len = strlen(main_string);
        
        printk("BTN3 in S0: len before = %u, temp_value = %c (%d)\n",
           (unsigned)len, (int)temp_value, (unsigned char)temp_value);


        main_string[len] = temp_value;
        main_string[len+1] = '\0';
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s1]);
        return SMF_EVENT_HANDLED;
    }



    return SMF_EVENT_HANDLED;
}

static void s0_exit(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
    btncode0 = 0;
    btncode1 = 0;
}


//s1 functions
static void s1_enter(void* o) {
    state_value = 1;
    temp_value = 0;

    temp_value = 0;

}

static enum smf_state_result s1_run(void* o) {

    if(standby_check()) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s3]);
        return SMF_EVENT_HANDLED;
    }
    
    if(led_state_object.count >= 125) {             //  1/8 s toggle to blink LED 3 at 4 hz
        led_state_object.count = 0;
        LED_toggle(LED3);
    }

    if(BTN_check_clear_pressed(BTN0)) {
        led_state_object.count = 0;
        btncode0 = 1;
        btncode1 = 0;
        temp_value <<= 1;
        temp_value |= 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s0]);
        return SMF_EVENT_HANDLED;
    }

    if(BTN_check_clear_pressed(BTN1)) {
        led_state_object.count = 0;
        btncode0 = 0;
        btncode1 = 1;
        temp_value <<= 1;
        temp_value |= 1;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s0]);
        return SMF_EVENT_HANDLED;
    }

    if(BTN_check_clear_pressed(BTN2)) {
        led_state_object.count = 0;
        main_string[0] = '\0';
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s0]);
        return SMF_EVENT_HANDLED;
    }

    if(BTN_check_clear_pressed(BTN3)) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s2]);
        return SMF_EVENT_HANDLED;
    }

    led_state_object.count++;
    return SMF_EVENT_HANDLED;
}


static void s1_exit(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
    printk("%c", temp_value);
}

//s2 functions
static void s2_enter(void* o) {
    state_value = 2;
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
    btncode0 = 0;
    btncode1 = 0;
}

static enum smf_state_result s2_run(void* o) {

    if(standby_check()) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s3]);
        return SMF_EVENT_HANDLED;
    }
    
    if(led_state_object.count >= 31.25) {             //  1/32 s toggle to blink LED 3 at 16 hz
        led_state_object.count = 0;
        LED_toggle(LED3);
    }
    
    if(BTN_check_clear_pressed(BTN3)) {
        led_state_object.count = 0;
        printk("[COMPLETE] Entered String:\n %s\n", main_string);
    }

    if(BTN_check_clear_pressed(BTN2)) {
        led_state_object.count = 0;
        main_string[0] = '\0';
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s0]);
        return SMF_EVENT_HANDLED;
    }

    led_state_object.count++;
    return SMF_EVENT_HANDLED;
}

static void s2_exit(void* o) {}

//s3 functions

static void s3_enter(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
    btncode0 = 0;
    btncode1 = 0;
}

static enum smf_state_result s3_run(void* o) {
    if(BTN_check_clear_pressed(BTN0) || BTN_check_clear_pressed(BTN1) || BTN_check_clear_pressed(BTN2) || BTN_check_clear_pressed(BTN3)) {
        if (state_value == 0) {
            smf_set_state(SMF_CTX(&led_state_object), &led_states[s0]);
            return SMF_EVENT_HANDLED;
        }
        if (state_value == 1) {
            smf_set_state(SMF_CTX(&led_state_object), &led_states[s1]);
            return SMF_EVENT_HANDLED;
        }
        if (state_value == 2) {
            smf_set_state(SMF_CTX(&led_state_object), &led_states[s2]);
            return SMF_EVENT_HANDLED;
        }
    }

    //using gaussian function for LED pwm cause it looks nice lol (researched from breathing LED projects online and found this)
    //from https://makersportal.com/blog/2020/3/27/simple-breathing-led-in-arduino,
    // y = (max value) * exp(-(x/N - Beta)^2/(2gamma^2))
    // N = adjusts (basically) the width of gaussian curve, i set it to more than the osc time so that it doesnt go fully dark
    // Beta = where within the width does it peak
    // gamma = it means how steeply it rises. a value of g = 0.14 is a sweet spot where it looks nice pulsing
    
    
    // played around in desmos: best values (so it looks nicer it doesnt go fully dark at each boundary):
    // N = 3, gamma = 0.14, Beta = 0.335, max value = 100 for PWM control
    // x is the time, so its led_state_object.count/1000 to make it in seconds

    //also looked it up: nrf52840 only has 1 floating point unit, so need to use float math instead of double.

    static uint8_t current_duty_cycle;

    float N = 3.0, Beta = 0.335, gamma = 0.14;
    float expf_numerator_unsquared = (led_state_object.count/1000.0f * 1/N) - Beta;
    float expf_denominator = 2.0f * gamma * gamma;

    current_duty_cycle = (uint8_t)(100 * expf(-expf_numerator_unsquared * expf_numerator_unsquared / expf_denominator)); //expf for float arithmetic

    LED_pwm(LED0, current_duty_cycle);
    LED_pwm(LED1, current_duty_cycle);
    LED_pwm(LED2, current_duty_cycle);
    LED_pwm(LED3, current_duty_cycle);

    if(led_state_object.count == 2000)
        led_state_object.count = 0;    

    led_state_object.count ++;
    return SMF_EVENT_HANDLED;
}



static void s3_exit(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
}
