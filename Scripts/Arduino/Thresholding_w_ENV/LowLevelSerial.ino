/*
General functions for the control of code based serial communication with the Arduino board
 */

void parse(char *line, char **argv, uint8_t maxArgs) {
	uint8_t argCount = 0;
	while (*line != '\0') {       /* if not the end of line ....... */
		while (*line == ',' || *line == ' ' || *line == '\t' || *line == '\n')
			*line++ = '\0';     /* replace commas and white spaces with 0    */
		*argv++ = line;          /* save the argument position     */
		argCount++;
		if (argCount == maxArgs-1)
			break;
		while (*line != '\0' && *line != ',' && *line != ' ' &&
				*line != '\t' && *line != '\n')
			line++;             /* skip the argument until ...    */
	}
	*argv = '\0';                 /* mark the end of argument list  */
}

//=======================
void RunSerialCom(int code) {

	switch (code) {

	case 86: // user defined command
		uint8_t c;
		msdelay(12);
		while (Serial.available() > 0) { // PC communication
			c = Serial.read();
			if (c == '\r') {
				buffer[idx] = 0;
				parse((char*)buffer, argv, sizeof(argv));
				
				//FV 1
				if (strcmp(argv[0], "fv") == 0) {
					if(strcmp(argv[1], "on") == 0) {
						valveOn(FINALVALVE);
						digitalWrite(FV_T,HIGH);
					}
					else if(strcmp(argv[1], "off") == 0) {
						valveOff(FINALVALVE);
						digitalWrite(FV_T,LOW);
					}
				}
       //FV 2
        else if (strcmp(argv[0], "fv2") == 0) {
          if(strcmp(argv[1], "on") == 0) {
            valveOn(FINALVALVE2);
            digitalWrite(FV_T,HIGH);
          }
          else if(strcmp(argv[1], "off") == 0) {
            valveOff(FINALVALVE2);
            digitalWrite(FV_T,LOW);
          }
        }
       //FV2 Auto
         else if (strcmp(argv[0], "fv2auto") == 0) {
          if(strcmp(argv[1], "on") == 0) {
          valveOnTimer(FINALVALVE2, 1000);
            digitalWrite(FV_T,HIGH);
          }
         }

       //needle open and close
        if (strcmp(argv[0], "ENV") == 0) {
          if(strcmp(argv[1], "open") == 0) {
            digitalWrite(ENV,HIGH);
          }
          else if(strcmp(argv[1], "close") == 0) {
            digitalWrite(ENV,LOW);
          }
        }
       //needle steps
        if (strcmp(argv[0], "step") == 0) {
          int step_num = atoi(argv[1]);
          for (int i=0; i<step_num; i++) {
            digitalWrite(ENV_STEP,HIGH);
            delay(5);
            digitalWrite(ENV_STEP,LOW);
            delay(5);
          }
        }

       //OPEN needle steps
        if (strcmp(argv[0], "open_step") == 0) {
          int step_num = atoi(argv[1]);
          digitalWrite(ENV,HIGH);
          for (int i=0; i<step_num; i++) {
            digitalWrite(ENV_STEP,HIGH);
            delay(5);
            digitalWrite(ENV_STEP,LOW);
            delay(5);
          }
          digitalWrite(ENV,LOW);
        }

       //CLOSE needle steps
        if (strcmp(argv[0], "close_step") == 0) {
          int step_num = atoi(argv[1]);
          digitalWrite(ENV,LOW);
          for (int i=0; i<step_num; i++) {
            digitalWrite(ENV_STEP,HIGH);
            delay(5);
            digitalWrite(ENV_STEP,LOW);
            delay(5);
          }
//          digitalWrite(ENV,LOW);
        }

				// water valve
				else if(strcmp(argv[0], "wv") == 0) {
					int water_valve = atoi(argv[1]);
					if(water_valve == 1) {
						if(strlen(argv[2]) > 0)
							waterdur = atoi(argv[2]);
						valveOnTimer(WATERVALVE1, waterdur);
					}
					else if(water_valve == 2) {
						if(strlen(argv[2]) > 0)
							waterdur2 = atoi(argv[2]);
						valveOnTimer(WATERVALVE2, waterdur2);
					}
				}

				// clean valve
				else if (strcmp(argv[0], "clean") == 0) {
					if(strcmp(argv[1], "on") == 0)
						valveOn(CLEARVALVE);
					else if(strcmp(argv[1], "off") == 0)
						valveOff(CLEARVALVE);
					else if(strlen(argv[1]) > 0) {
						unsigned long clean_dur = atoi(argv[1]);
						if((clean_dur < 0) || (clean_dur > 10000))
							clean_dur = 2000;
						valveOnTimer(CLEARVALVE, clean_dur);
					}
				}
				// water callibration (100 water drops)
				else if(strcmp(argv[0], "callibrate") == 0) {
					int water_valve = atoi(argv[1]);
					if(strlen(argv[2]) > 0) {
						if(water_valve == 1)
							waterdur = atoi(argv[2]);
						else if(water_valve == 2)
							waterdur2 = atoi(argv[2]);
					}
					for(int x=0; x<=100; x=x+1){
						if(water_valve == 1)
							valveOnTimer(WATERVALVE1, waterdur);
						else if(water_valve == 2)
							valveOnTimer(WATERVALVE2, waterdur2);
						//Serial1.print(0x0c, BYTE);
						delay(1000);
						Serial1.write(0x94);
						Serial1.print("CAL: ");
						Serial1.print(x);
					}
				}
				else if(strcmp(argv[0], "waterdur") == 0) {
					int water_valve = atoi(argv[1]);
					if(water_valve == 1)
						waterdur = atoi(argv[2]);
					else if(water_valve == 2)
						waterdur2 = atoi(argv[2]);
				}
				else if(strcmp(argv[0], "blocks") == 0) {
					if(strcmp(argv[1], "on") == 0)
						enableblocks = true;
					else if(strcmp(argv[1], "off") == 0)
						enableblocks = false;
					if(strlen(argv[2]) > 0)
						blocks = atoi(argv[1]);
				}
				else if(strcmp(argv[0], "Laser") == 0) {
					if(strcmp(argv[2], "trigger") == 0) {
						unsigned long laser_dur;
						uint16_t laser_amp;
						uint8_t laser_chan;
						char **endptr;
						laser_chan = (uint8_t)atoi(argv[1]);
						laser_amp = atoi(argv[3]);
						laser_dur = strtoul(argv[4],endptr,0)*1000;
						setPulse(laser_chan,laser_dur,laser_amp);
						trigPulse(laser_chan);
					}
				}
			else if(strcmp(argv[0], "threemissed") == 0) {
				if(strcmp(argv[1], "on") == 0)
				{
          digitalWrite(THREEMISSED_LED, HIGH); //Turn on a LED to indicate 3 missed in a row
				}
        if(strcmp(argv[1], "off") == 0)
        {
          digitalWrite(THREEMISSED_LED, LOW);
        }
      }
				idx = 0;
			}
			else if (((c == '\b') || (c == 0x7f)) && (idx > 0))
				idx--;
			else if ((c >= ' ') && (idx < sizeof(buffer) - 1))
				buffer[idx++] = c;
		}
		Serial.print(2);
		Serial.print(",");
		Serial.println("*");
		//      Serial.flush();
		break;


	case 87: // data has been requested

		unsigned long currenttime;
		// transmit buffer
		if((trialdone) && ((lastsent) || !hasLickdata())) {
			// send end code
			Serial.print(5);
			Serial.print(",");
			Serial.println("*");
			//        Serial.flush();
			trialdone = false;
			lastsent = false;
			break;
		}

		else if(trialdone) {
			// prepare to send last packet
			lastsent = true;
		}

		Serial.print(6);
		Serial.print(",");

		// transmit sniff signal.
		// TODO: Check for sniff buffer overflow
		currenttime = totalms;
		cur_sniffndx = currentvalue;
		/* A timer tick might occur between the above two assignments, incrementing totalms and
         misaligning timestamp and values written to buffer. The following while
         loop makes sure both assignments occur with no time tick in between the statements */
		while(currenttime != totalms) {
			currenttime = totalms;
			cur_sniffndx = currentvalue;
		}
		// calculate number of bytes to send for sniff and lick.
		int sniff_sample_number; // number of sniff samples
		int sniff_bytes;
		sniff_sample_number = (cur_sniffndx+BUFFERS-last_sniffndx)%BUFFERS;
		sniff_bytes = sniff_sample_number * 2;
		// ----- Calculate the number of lick values to send and the start and stop indices ------
		// ---LICK1---
		int tail1, head1, numlicks1,lickbytes1;
		bool lick_signal_state1;
		tail1 = licktail;
		head1 = lickhead;
		lick_signal_state1 = beamstatus;
		// make sure the above two variables were assigned at the same millisecond
		while(head1 != lickhead) {
			head1 = lickhead;
			lick_signal_state1 = beamstatus;
		}
		if (lick_signal_state1){
			//if licking currently, won't transmit the last value, which is lick on with no off
			head1 = head1 - 1;
			if (head1 < 0){
				head1 = LICKBUFF + head1;
			}
		}
		if(head1 < tail1){
			numlicks1 = (LICKBUFF - tail1) + head1;
		}
		else
			numlicks1 = head1-tail1;
		lickbytes1 = numlicks1*4;

		// ---LICK2---
		int tail2, head2, numlicks2,lickbytes2;
		bool lick_signal_state2;
		tail2 = lick2tail;
		head2 = lick2head;
		lick_signal_state2 = beam2status;
		// make sure the above two variables were assigned at the same millisecond
		while(head2 != lick2head) {
			head2 = lick2head;
			lick_signal_state2 = beam2status;
		}
		if (lick_signal_state2){
			head2 = head2 - 1;
			if (head2 < 0){
				head2 = LICKBUFF + head2;
			}
		}
		if(head2 < tail2)
			numlicks2 = (LICKBUFF - tail2) + head2;
		else
			numlicks2 = head2-tail2;
		lickbytes2 = numlicks2 * 4;

		// SEND HEADER INFORMATION: NUMBER STREAMS, NUMBER OF PACKETS PER STREAM

		Serial.print(5); // number of streams to send
		Serial.print(",");
		Serial.print(4); //number of bytes for stream 1 (packet_sent_time)
		Serial.print(",");
		Serial.print(2); //number of bytes for sniff_samples (single integer)
		Serial.print(",");
		Serial.print(sniff_bytes); //number of bytes for actual sniff stream.
		Serial.print(",");
		Serial.print(lickbytes1);
		Serial.print(",");
		Serial.print(lickbytes2);
		Serial.println(",");
		// end line here so that python will read and parse the handshake. Then...
		// ----- SEND ACTUAL DATA AS BINARY -----------------
		sendLongAsBytes(currenttime);
		sendShortAsBytes(sniff_sample_number);
		while(last_sniffndx != cur_sniffndx) {
			last_sniffndx = (last_sniffndx+1)%BUFFERS;
			sendShortAsBytes(sniff[last_sniffndx]);
		}

		//		// trigger signal
		//		if(trighead != trigtail) {
		//			int head, tail;
		//			head = trighead;
		//			tail = trigtail;
		//
		//			for (int i=tail; i!=head;) {
		//				Serial.print(trig[i]);
		//				i = (i+1)%TRIGBUFF;
		//				if(i == head)
		//					Serial.print(",");
		//				else
		//					Serial.print(";");
		//			}
		//			trigtail = head;
		//		}
		//		else
		//			Serial.print(","); // empty signal
		////////

		while(tail1!=head1)  {
			sendLongAsBytes(lick[tail1]);
			tail1 = (tail1+1)%(LICKBUFF);
//			Serial1.print(tail1);
		}
		licktail = tail1;
		// move the ring buffer tail index to current untransmitted tail index

		while (tail2!=head2){
			sendLongAsBytes(lick2[tail2]);
			tail2 = (tail2+1)%(LICKBUFF);
		}
		lick2tail = tail2;



		//      // treadmill signal. Currently, is acquired at same rate as sniff signal
		//      if(last_treadmillndx == cur_sniffndx)
		//        Serial.print(",");  // empty signal
		//      while(last_treadmillndx != cur_sniffndx) {
		//        last_treadmillndx = (last_treadmillndx+1)%BUFFERS;
		//        Serial.print(treadmill[last_treadmillndx]);
		//        if(last_treadmillndx == cur_sniffndx)
		//          Serial.print(",");
		//        else
		//          Serial.print(";");
		//      }

		// end of packet signal
		//      Serial.flush();
		break;

	case 88: // trail ended and the trial details were requested
		// tell the monitor about the trial details
		Serial.print(4);
		Serial.print(",");
		Serial.print(result);
		Serial.print(",");
		Serial.print(paramsgottime);
		Serial.print(",");
		Serial.print(starttrial);
		Serial.print(",");
		Serial.print(endtrial);
		Serial.print(",");
		Serial.print(no_sniff);
		Serial.print(",");
		Serial.print(fvOnTime);
		Serial.print(",");
		Serial.println("*");
		//      Serial.flush();
		break;

	case 90: // start trial (i.e. need to read from the serial port)
		//received_params = 0;
		//delay(10);
		rchoice =  readULongFromBytes();
		//delay(10);
		waterdur = readULongFromBytes();
		waterdur2 = readULongFromBytes();
		//delay(10);
		fvdur = readULongFromBytes();
		trialdur = readULongFromBytes();
		iti = readULongFromBytes();
//		fvonset = readULongFromBytes();
//		treadmill_response_type = readULongFromBytes();
		grace_period = readULongFromBytes();
		rewards_given = readULongFromBytes();

		Serial.print(2);
		Serial.print(",");
		Serial.println("*");
		//      Serial.flush();
		break;

	case 91:
		Serial.print(6);
		Serial.print(",");
		Serial.print(protocolName);
		Serial.print(",");
		Serial.println("*");
		//      Serial.flush();
		break;

		//    case 92: // directly control the arduino pin settings
		//
		//
		//      break;
	}
}
//=======================

int readIntFromBytes() {

	union u_tag {
		byte b[2];
		int ival;
	} u;

	u.b[0] = Serial.read();
	u.b[1] = Serial.read();

	return u.ival;
}

byte readbyte() {

	unsigned long temp1;
	byte value;
	temp1 = totalms;
	// attempt reading the serial for 200ms
	while(totalms-temp1 < 2000)  {
		if(Serial.available() > 0)
			return Serial.read();
		//value = Serial.read();
		//if(value != 255)
		//  return value;
	}
	Serial1.write(0x0c); // clear the display
	delay(10);
	Serial1.write(0x80); // col 0, row 0
	Serial1.print(" Timeout in Serial ");
	Serial1.print("Params read: ");
	//Serial1.print(received_params);
	return 0;
}

unsigned long readULongFromBytes() {

	union u_tag {
		byte b[4];
		unsigned long ulval;
	} u;

	u.b[0] = readbyte();
	u.b[1] = readbyte();
	u.b[2] = readbyte();
	u.b[3] = readbyte();

	// debugging value indicating number of longs read
	//received_params++;
	return u.ulval;
}
