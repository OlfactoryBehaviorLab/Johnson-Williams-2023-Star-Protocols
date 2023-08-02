

// include the library code:
#include <SPI.h>
#include <C:\Voyeur_protocols_and_files\Arduino\libraries\voyeur_timer_lib.pde>
#include <C:\Voyeur_protocols_and_files\Arduino\libraries\ioFunctions_external_timers.pde>
#include <C:\Voyeur_protocols_and_files\Arduino\libraries\voyeur_serial_stream_tools.pde>
#include <C:\Voyeur_rig_config\arduino_rig_config.h>

#define INITIALGO 0
#define GO 1
#define NOGO 2

#define PASSIVE 4
#define PASSIVEREWARDED 3
#define POSITIVE 1
#define NEGATIVE 0
#define FV 3
#define TRIAL_LED CUE3
#define THREEMISSED_LED CUE4
#define FVSYNC DIGITAL7

//=======================
// Set the protocol name
char protocolName[] = "ThresholdingENV"; // should be less than 20 characters
//=======================

//=======================
// PARAMETERS OF TASK
unsigned long waterdur = 60;
unsigned long waterdur2 = 60;
unsigned long trialdur = 1000000;
unsigned long sniffmaxdelay = 0;
unsigned long rchoice = INITIALGO;
unsigned long fvdur = 0;
unsigned long grace_period = 0;
unsigned long firststim = 0;
unsigned long iti = 5000;
//unsigned long lickfirst = 1;
unsigned long fvonset = 0;
unsigned long lickStartTime = 0;



//event to be transmitted back
unsigned int result = 0;
unsigned long paramsgottime = 0;
unsigned long starttrial = 0;
unsigned long endtrial = 0;
unsigned long firstlick = 0;
unsigned long firstcorrectlick = 0;
unsigned int no_sniff = 0;
unsigned long fvOnTime = 0;
unsigned long vialOnTime = 0;
unsigned long laserontime = 0;

// variables
uint8_t idx = 0, fv_routine_timer = 255, laser_routine_timer = 255, led_timer = 255;
uint16_t lickindex;  // lick index
int blocks;
bool trialdone = false, lastsent = false, enableblocks = false, maskon = false, ledMaskwFV = true,trialwait = false;
volatile bool trig_stop = false; bool beam1state, beam2state;
// state of the state machine
int code = 0, state = 0, trig_laser_multi_sniff = 0;
unsigned long temp1 = 0, inh_onset = 0, lastlickstamp = 0, lastlick1 = 0, lickcheckpoint=0;
int last_sniffndx = -1, cur_sniffndx = 0;
unsigned long nextpulsedelay = 0;
int treadmill_response_type = 0, rewards_given = 0;

//clean count iterates how many times the program will attempt to clean the mouse within a single session.
//lastCleanTime is the last time the cleaning iteration was started.

char buffer[128];
char *argv[8];
//bool received_params = 0;
//char laserdur[20], laseramp[20];

int step_num = 0;










void setup() {
  pinMode(LED_PIN, OUTPUT); // Green LED on the front
  pinMode(TRIGGER1, OUTPUT); // Pulse generator ch. 1 trigger
  pinMode(TRIGGER2, OUTPUT); // Pulse generator ch. 2 trigger
  pinMode(SOLENOID1, OUTPUT);
  pinMode(SOLENOID2, OUTPUT);
  pinMode(SOLENOID3, OUTPUT);
  pinMode(SOLENOID4, OUTPUT);
  pinMode(SOLENOID5, OUTPUT);
  pinMode(SOLENOID6, OUTPUT);
  pinMode(SOLENOID7, OUTPUT);
  pinMode(SOLENOID8, OUTPUT);
  pinMode(DIGITAL1,OUTPUT);
  pinMode(DIGITAL2,OUTPUT);
  pinMode(DIGITAL3,OUTPUT);
  pinMode(DIGITAL4,OUTPUT);
  pinMode(DIGITAL5,OUTPUT);
  pinMode(DIGITAL6,OUTPUT);
  pinMode(DIGITAL7,OUTPUT);
  pinMode(DIGITAL8,OUTPUT);
  pinMode(DIGITAL9,OUTPUT);  
  pinMode(DIGITAL10,OUTPUT);
  pinMode(DIGITAL11,OUTPUT);
  pinMode(DIGITAL12,OUTPUT);
  pinMode(CUE1, OUTPUT);
  pinMode(CUE2, OUTPUT);
  pinMode(CUE3, OUTPUT);
  pinMode(CUE4, OUTPUT);
  pinMode(CUE5, OUTPUT);
  pinMode(CUE6, OUTPUT);
  pinMode(CUE7, OUTPUT);
  pinMode(CUE8, OUTPUT);
  pinMode(ADC_PIN, OUTPUT);
  pinMode(DAC1_PIN, OUTPUT);
  pinMode(DAC2_PIN, OUTPUT);
  pinMode(TEENSY_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(TRIGGER1, LOW);
  digitalWrite(TRIGGER2, LOW);
  digitalWrite(SOLENOID1, LOW);
  digitalWrite(SOLENOID2, LOW);
  digitalWrite(SOLENOID3, LOW);
  digitalWrite(SOLENOID4, LOW);
  digitalWrite(SOLENOID5, LOW);
  digitalWrite(SOLENOID6, LOW);
  digitalWrite(SOLENOID7, LOW);
  digitalWrite(SOLENOID8, LOW);
  digitalWrite(DIGITAL1,LOW);
  digitalWrite(DIGITAL2,LOW);
  digitalWrite(DIGITAL3,LOW);
  digitalWrite(DIGITAL4,LOW);
  digitalWrite(DIGITAL5,LOW);
  digitalWrite(DIGITAL6,LOW);
  digitalWrite(DIGITAL7,LOW);
  digitalWrite(DIGITAL8,LOW);
  digitalWrite(DIGITAL9,LOW);
  digitalWrite(DIGITAL10,LOW);
  digitalWrite(DIGITAL11,LOW);
  digitalWrite(DIGITAL12,LOW);
  digitalWrite(CUE1, LOW);
  digitalWrite(CUE2, LOW);
  digitalWrite(CUE3, LOW);
  digitalWrite(CUE4, LOW);
  digitalWrite(CUE5, LOW);
  digitalWrite(CUE6, LOW);
  digitalWrite(CUE7, LOW);
  digitalWrite(CUE8, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(ADC_PIN, HIGH);
  digitalWrite(DAC1_PIN, HIGH);
  digitalWrite(DAC2_PIN, HIGH);
  digitalWrite(TEENSY_PIN, HIGH);

  //=======================
  // prep ANALOG inputs
  analogReference(DEFAULT);
  //=======================

  //=======================
  // prep SPI for AD control
  startSPI();
  //=======================

  //=======================
  // initialize the SERIAL communication
  Serial.begin(115200);
  //Serial.begin(921600);

  //Serial.println("* Monitor System ready *");
  //=======================

  //=======================
  // initialize SERIAL for LCD
  Serial1.begin(19200); 
  Serial1.write(0x0c); // clear the display
  delay(10);
  Serial1.write(0x11); // Back-light on
  Serial1.write(0x80); // col 0, row 0
  Serial1.print("*CleanThresholding   Dewan Lab*");
  delay(20);
  //    Serial1.print(0x94, BYTE); // col 0, row 0
  //    Serial1.print("MODE 0");
  //=======================

  // Pulse generator communication
  Serial2.begin(115200);
  setupVoyeurTimer(); //setup and start ms timer (From: voyeur_timer_lib.pde) (THIS DEFINES TOTALMS).

  // setup buffer sizes
  setupBuffers(400,1,20,50);  //(ioFunctions)(unsigned int sniffbuff, unsigned int treadmillbuff, unsigned int lickbuff, unsigned int trigbuff)

  // recording of both sniff and velocity
  // first two args are analog channels' pins, 1 indicates ONLY SNIFF ON, 3 indicates both on.
  setupAnalog(0,1,1); 

  // start analog acquisition
  start_analog_timer(); //(ioFunctions).
  // start lick recording timer.
  // first arg. is the beam break pin, second is the amount of ms to wait. 0 = every ms
  startLick(BEAM1,BEAM2);//(ioFunctions).
  lickOn(3, 0); // 3 - turn on both lick pins, 0 is frequency (every ms). (ioFunctions).
  recordsniffttl = true;

  // Init done
  digitalWrite(LED_PIN, HIGH);
}

//===================================================================================================
void loop() {  //BREAKS TO HERE

  //===================================================================================================
  //================ State machine goes here===========================================================
  unsigned long timenow = totalms; //copy this to register from volatile.



  switch (state) {

  case 0: //waiting for initialization of trial
    break;



  case 1: // state was changed to 1 via controller (serial).
//    if((timenow-endtrial) < iti)  // wait for the inter-trial interval to be over (difference between last trial and the current time)
//      break;
//    else{
      vialOnTime = timenow;
      state = 2;
//    }
    break;

  case 2: // Wait for odor to reach max
    if((timenow-vialOnTime) < 900)  // 
      break;
    else{
      state = 3;
      digitalWrite(TRIAL_LED,HIGH); //Turn on Trial Light
      starttrial = timenow;
    }
    break;


  case 3:  // FINAL VALVE     If the final valve gets turn on, turn on the final valve for fdur time and mark the time

    valveOn(FINALVALVE);
    fvOnTime = timenow;
    digitalWrite(FVSYNC, HIGH);
    digitalWrite(ENV,LOW);
    
//    step_num = 40;
//    for (int i=0; i<step_num; i++)
//    {
//      digitalWrite(ENV_STEP,HIGH);
//      delay(2);
//      digitalWrite(ENV_STEP,LOW);
//      delay(2);
//    }
            
    state = 4;
    break;

  case 4:  //TRIAL TYPE.
    if((timenow-fvOnTime) < grace_period) // wait fo
    {
        break;
    }
        
    else{
          lickStartTime = timenow;  //mark start of lick time
          if(rchoice == INITIALGO) //Initial Go trials
              state = 5;
          else if(rchoice == GO) //GO TRIALS
              state = 6;
          else if(rchoice == NOGO) // NOGO TRIALS
              state = 7;    
     }
     break; 

  case 5:  // INITIAL GO TRIAL     

      //OPTION 1: Trial time ends with nothing happening
      if(timenow - starttrial > trialdur) { //Trial is over; nothing happened
      result = 5; // End Trial
      state = 10;
      break;
    }

      //OPTION 2: Animal Licks and receives water
      else if (haslicked(1,lickStartTime)){ //correct
      result = 1;
      valveOnTimer(WATERVALVE1, waterdur);
      firstcorrectlick = getLastLick(1);
      state = 8;
    }
    break;


  case 6:  //GO TRIAL      

      //OPTION 1: Trial time ends with nothing happening
    if(timenow - starttrial > trialdur) { //Trial is over; nothing happened
      result = 5;
      state = 10;
      break;
    }
   
  //OPTION 3: Animal Licks and receives water
    else if (haslicked(1,lickStartTime)){ //correct
      result = 1;
      valveOnTimer(WATERVALVE1, waterdur);
      firstcorrectlick = getLastLick(1);
      state = 8;
    }

    break;


  case 7:  // NOGO TRIAL
        //OPTION 1: Trial time ends with nothing happening ANIMAL IS CORRECT
    if(timenow - starttrial > trialdur) {
      state = 10;
      result = 2; //CORRECT REJECTION
      break;
    }

    // FALSE ALARM
    if (haslicked(1,lickStartTime)) { 
      result = 3;
      firstcorrectlick = timenow;
      state = 8;
    }
    break;

  case 8:  // Place holder to add time to wait for the end of the trial. Currently, trial is over after animal licks
    state = 10;
    break;


  case 10: // turn off everything, end trial, state 0
    valveOff(FINALVALVE);
    digitalWrite(FVSYNC, LOW);   
    digitalWrite(ENV,HIGH);
//    for (int i=0; i<step_num+20; i++) 
//    {
//      digitalWrite(ENV_STEP,HIGH);
//      delay(2);
//      digitalWrite(ENV_STEP,LOW);
//      delay(2);
//    }
            

    digitalWrite(TRIAL_LED,LOW); //Turn off Trial Light
    endtrial = totalms;
    trialdone = true;
    iti = 0;
    state = 0;
    break;

  }



  
  //===================================================================================================
  // CHECK SERIAL PORT
  //===================================================================================================


  if (Serial.available() > 0) {

    code = Serial.read();

    switch (code) {
    case 89: // stop execution
      state = 0;
      if(fvdur == 0) {
        valveOff(FINALVALVE);
        digitalWrite(FV_T,LOW);
      }
      Serial.print(3);
      Serial.print(",");     
      Serial.print("*");
      break;

      // all other codes are handshaking codes either requesting data or sending data 
    case 87:
      RunSerialCom(code); 
      break;

    case 88:
      RunSerialCom(code); 
      break;

    case 86:
      RunSerialCom(code);
      break;

    case 90:
      RunSerialCom(code);
      state = 1;
      paramsgottime = totalms;
      firstlick = firstcorrectlick = 0;
      starttrial = no_sniff = 0;
      laserontime = 0;
//      Serial1.write(0x0c);
//      delay(10);
//      Serial1.write(0x80); // col 0, row 0
//      Serial1.print('lick_training. Rewards: ');
//      Serial1.print(rewards_given);


      break;

    case 91:
      RunSerialCom(code);
      break;

    case 92:
      digitalWrite(SOLENOID1, HIGH);
      delay(5000);
      digitalWrite(SOLENOID1, LOW);
      break;
    }
  }
}
