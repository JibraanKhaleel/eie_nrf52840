/**
 * @file my_state_machine.c
 */

#include <zephyr/smf.h>

#include "LED.h"
#include "my_state_machine.h"

//prototypes (commented out unneededstuff from lesson)

/*
static void led_on_state_exit(void* o);
static enum smf_state_result led_on_state_run(void* o);

static void led_off_state_exit(void* o);
static enum smf_state_result led_off_state_run(void* o);
*/

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

    //S4 prototypes
static void s4_enter(void* o);
static void s4_exit(void* o);
static enum smf_state_result s4_run(void* o);

//typedefs (commented out unneeded stuff from lesson)

/*
enum led_state_machine_states{
    LED_ON_STATE,
    LED_OFF_STATE
};
*/

enum led_state_machine_states{
    s0,
    s1,
    s2,
    s3,
    s4
};
typedef struct {
    //context var used to track state machine state, must be first
    struct smf_ctx ctx;
    uint16_t count;
} led_state_object_t; //defining led_state_object type





//Local vars (commented out unnecessary stuff from Lesson)
/*
static const struct smf_state led_states[] = {
    [LED_ON_STATE] = SMF_CREATE_STATE(NULL, led_on_state_run, led_on_state_exit, NULL, NULL),
    [LED_OFF_STATE] = SMF_CREATE_STATE(NULL, led_on_state_run, led_on_state_exit, NULL, NULL)
}
*/

static const struct smf_state led_states[] = {
    [s0] = SMF_CREATE_STATE(s0_enter, s0_run, s1_exit, NULL, NULL),
    [s1] = SMF_CREATE_STATE(s1_enter, s1_run, s1_exit, NULL, NULL),
    [s2] = SMF_CREATE_STATE(s2_enter, s2_run, s2_exit, NULL, NULL),
    [s3] = SMF_CREATE_STATE(s3_enter, s3_run, s3_exit, NULL, NULL),
    [s4] = SMF_CREATE_STATE(s4_enter, s4_run, s4_exit, NULL, NULL)
};

static led_state_object_t led_state_object;

/*
void state_machine_init() {
    led_state_object.count = 0;
    smf_set_initial(SMF_CTX(&led_state_object), &led_states[LED_ON_STATE]);
}
*/

void state_machine_init() {
    led_state_object.count = 0; //accesses the "count" within the struct named led_state_object of type led_state_object_t, makes it =0
    smf_set_initial(SMF_CTX(&led_state_object), &led_states[s0]); //Initializes state machine in State 0
}


int state_machine_run() {
    return smf_run_state(SMF_CTX(&led_state_object));
}


//s0 functions
static void s0_enter(void* o) {
    LED_SET(LED0, LED_OFF);
    LED_SET(LED1, LED_OFF);
    LED_SET(LED2, LED_OFF);
    LED_SET(LED3, LED_OFF);
}

static enum smf_state_result s0_run(void* o) {
    if(BTN_check_clear_pressed(BTN0)) {    
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s1]);
        return SMF_EVENT_HANDLED;
    }
    else {
        led_state_object.count++;   
    }

    return SMF_EVENT_HANDLED;
}

static void s0_exit(void* o) {}


//s1 functions
static void s1_enter(void* o) {}
static enum smf_state_result s1_run(void* o) {

    if(led_state_object.count >= 125) {
        led_state_object.count = 0;
        LED_toggle(LED0);
    }

    if(BTN_check_clear_pressed(BTN1)) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s2]);
    }

    if(BTN_check_clear_pressed(BTN2)) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s3]);
    }
    
    led_state_object.count++;
}
static void s1_exit(void* o) {
    LED_SET(LED0, LED_OFF);
}

//s2 functions
static void s2_enter(void* o) {
    LED_SET(LED0, LED_ON);
    LED_SET(LED1, LED_OFF);
    LED_SET(LED2, LED_ON);
    LED_SET(LED3, LED_OFF);
}

static enum smf_state_result s2_run(void* o) {
    if(BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[s0]);
    }
}


/*
static void led_on_state_exit(void* o) {
    LED_set(LED0, LED_OFF);
}

static enum smf_state_result led_on_state_run(void* o) {
    if (led_state_object.count > 500) {
        led_state_object.count  = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[LED_OFF_STATE]);
    } else {
        led_state_object.count++;
    }

    return SMF_EVENT_HANDLED;
}

static void led_off_state_exit(void* o) {
    LED_set(LED0, LED_ON);
}

static enum smf_state_result led_off_state_run(void* o) {
    if (led_state_object.count > 500) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[LED_ON_STATE]);
    } else {
        led_state_object.count++;
    }
    
    return SMF_EVENT_HANDLED;

}

*/