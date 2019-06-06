// Musical synthesizer with saxophone fingering

// This program is intended for the circuit described in Connections_V7.pdf.
// Programmed for Arduino Uno

// In the main loop of the program, the keyboard diode matrix is read, writing consecutively
// a logical 1 into each matrix row, and reading the 4 bits of the columns each time. 
// If a change is detected in the keyboard state with respect to the state previously stored, 
// the new note is processed, otherwise the keyboard reading is repeated.
// To process the new note, with the 24 bits obtained, a word is formed that is searched 
// in the list of note codes. The index of the note found is used to obtain the MIDI code of the note.
// If the mouthpiece is been blowed, the note is played.
// Other changes are also checked in the loop, such as tone change, instrument change, pitch bend or volume change.

// Author: Danel Sanchez Tarrago
// Colaborator: Raul Sanchez Perez
// Colaborator: Raul Sanchez Patiño
// 2016 - 2019

// 
// TABLE OF SELECTED MIDI INSTRUMENTS
//

// PIANO
const byte PIANO           = 1;   // Acoustic Grand Piano
const byte BRIGHT_PIANO    = 2;   // Bright Acoustic Piano
const byte HARPSICHORD     = 7;
const byte CLAVI           = 8;

// CHROMATIC PERCUSION
const byte CELESTA         = 9;   
const byte GLOCKENSPIEL    = 10;
const byte VIBRAPHONE      = 12;  
const byte MARIMBA         = 13;
const byte XYLOPHONE       = 14;
const byte TUBULAR_BELLS   = 15;

// ORGAN
const byte DRAWBAR_ORGAN   = 17;  
const byte CHURCH_ORGAN    = 20;
const byte REED_ORGAN      = 21;
const byte ACCORDION       = 22;
const byte HARMONICA       = 23;
const byte TANGO_ACCORDION = 24;

// GUITAR
const byte ACOUSTIC_GUITAR   = 25;  // Nylon
const byte JAZZ_GUITAR       = 27;  // ELECTRIC JAZZ GUITAR
const byte CLEAN_GUITAR      = 28;  // ELECTRIC CLEAN GUITAR
const byte OVERDRIVEN_GUITAR = 30;
const byte DISTORTION_GUITAR = 31;
const byte GUITAR_HARMONICS  = 32;

// BASS
const byte ACOUSTIC_BASS   = 33;
const byte FINGER_BASS     = 34;   // ELECTRIC FINGER BASS 
const byte SLAP_BASS_1     = 37;
const byte SLAP_BASS_2     = 38;
const byte SYNTH_BASS_1    = 39;
const byte SYNTH_BASS_2    = 40;

// STRING
const byte VIOLIN          = 41;
const byte VIOLA           = 42; 
const byte CELLO           = 43;
const byte CONTRABASS      = 44; 
const byte TREMOLO_STRINGS = 45;
const byte PIZZICATO_STRINGS = 46;
const byte ORCHESTRAL_HARP = 47;
const byte TIMPANI         = 48;

// ENSEMBLE
const byte STRING_ENSEMBLE = 49;  
const byte ORCHESTRA_HIT   = 56;

// BRASS
const byte TRUMPET         = 57;
const byte TROMBONE        = 58;  
const byte TUBA            = 59;
const byte MUTED_TRUMPET   = 60;
const byte FRENCH_HORN     = 61;  
const byte BRASS_SECTION   = 62;  

// REED
const byte SOPRANO_SAX     = 65;
const byte ALTO_SAX        = 66;  
const byte TENOR_SAX       = 67;
const byte BARITONO_SAX    = 68;
const byte OBOE            = 69;
const byte ENGLISH_HORN    = 70;
const byte BASSOON         = 71;  
const byte CLARINET        = 72;  

// PIPE
const byte PICCOLO         = 73;
const byte FLUTE           = 74;
const byte PAN_FLUTE       = 76;
const byte BLOWN_BOTTLE    = 77;
const byte SHAKUHACHI      = 78;
const byte WHISTLE         = 79;
const byte OCARINA         = 80;

// SYNTH PAD
const byte NEW_AGE         = 89;
const byte WARM_PAD        = 90;
const byte CHOIR           = 92;
const byte METALLIC        = 94;
const byte HALO            = 95;

// SYNTH FX
const byte CRYSTAL         = 99;
const byte ATMOSPHERE      = 100;
const byte BRIGHTNESS      = 101;
const byte GOBLINS         = 102;
const byte SCI_FI          = 104;

// ETHNIC
const byte SITAR           = 105;
const byte BANJO           = 106;
const byte KALIMBA         = 109;
const byte GAITA           = 110;
const byte FIDDLE          = 111;
const byte SHEHNAI         = 112;

// PERCUSSIVE
const byte TINKLE_BELL     = 113;
const byte AGOGO           = 114;
const byte PITCHED_PERCUSS = 115;
const byte WOODBLOCK       = 116;
const byte TAIKO_DRUM      = 117;
const byte MELODIC_TOM     = 118;

//
// SET UP HERE THE 16 INSTRUMENTS FOR THE SINTHESIZER
//
const byte pitchNumber = 16;
const byte pitchTable[pitchNumber] = {
/* 01 */                              ALTO_SAX,                
/* 02 */                              CLARINET, 
/* 03 */                              FLUTE, 
/* 04 */                              BASSOON, 
/* 05 */                              TROMBONE, 
/* 06 */                              FRENCH_HORN, 
/* 07 */                              VIOLA, 
/* 08 */                              CONTRABASS,
/* 09 */                              PIANO, 
/* 10 */                              DRAWBAR_ORGAN, 
/* 11 */                              ACOUSTIC_GUITAR, 
/* 12 */                              CLEAN_GUITAR,
/* 13 */                              CELESTA, 
/* 14 */                              VIBRAPHONE, 
/* 15 */                              STRING_ENSEMBLE, 
/* 16 */                              BRASS_SECTION
                                      };


#include <SoftwareSerial.h>

//
// Pin configuration
//
const byte pitchSel = 0; // Indicador del modo Activado/Desactivado de selección de timbres (Cable Gris) Digital pin 0 (salida)
const byte principalInst = 1; // Indicador del instrumento Principal/Secundario seleccionado (Cable Rosado) Digital pin 1 (salida)
const byte VS1053_RX = 2; // Connects to the RX pin on Adafruit (Cable Morado) Digital pin 2
const byte pulseLane0 = 3; // Pulso 0 (Cable Azul) Digital pin 3 (salida)
const byte pulseLane1 = 4; // Pulso 1 (Cable Amarillo) Digital pin 4 (salida)
const byte pulseLane2 = 5; // Pulso 2 (Cable Verde) Digital pin 5 (salida)
const byte pulseLane3 = 6; // Pulso 3 (Cable Marrón) Digital pin 6 (salida)
const byte demux = 7; // Activa/Desactiva el demultiplexor (Cable Naranja) Digital pin 7 (salida)
const byte controlBank1 = 8; // Lectura del Banco de Controles 1 (Cable Olivo) Digital pin 8 (entrada)
const byte VS1053_RESET = 9; // Connects to the RESET pin on Adafruit (Cable Rojo Indio) Digital pin 9
const byte controlBank2 = 10; // Lectura del Banco de Controles 2 (Cable Verde Intermitente) Digital pin 10 (entrada)
const byte BFlatKeyIndicator = 11; // Indicates C Key selection (Dashed Orange) Digital pin 11 (OUTPUT)
const byte EFlatKeyIndicator = 12; // Indicates E flat Key selection (Dashed Blue) Digital pin 12 (OUTPUT)
const byte CKeyIndicator = 13; // Indicates B flat Key selection (Dashed Pink) Digital pin 13 (OUTPUT)
const byte matrix0 = A0; // Matriz 0 (Cable Azul Intermitente) Analog pin 0 (entrada)
const byte matrix1 = A1; // Matriz 1 (Cable Amarillo Intermitente) Analog pin 1 (entrada)
const byte matrix2 = A2; // Matriz 2 (Cable Verde Intermitente) Analog pin 2 (entrada)
const byte matrix3 = A3; // Matriz 3 (Cable Marrón Intermitente) Analog pin 3 (entrada)
const byte volumePin = A4; // Lectura del Control de Volumen (Cable Morado Intermitente) Analog pin 4 (entrada)
const byte pitchBendPin = A5; // Lectura del Control de Portamento (Cable Salmón Intermitente) Analog pin 5 (entrada)

//
//  Keyboard variables and constants
//
const unsigned long persistentChangeLength = 10000; // esperar 10 milisegundos antes de dar la nota (cuando hay cambios en el teclado)
unsigned long keyboardChangeStart;  // Momento en que ocurre un cambio en el teclado
boolean keyboardPersistentChange = false;
boolean keyboardPreviouslyChanged = false; // variable global, su valor persiste hasta que comienza el sonido
boolean playingMusic = false;
byte keyboardCurrentState[4]; // Lectura actual del teclado
byte keyboardPreviousState[4] = {65, 65, 65, 65}; // Lectura previa del teclado
int previousNote = 0;

//
// Keyboard - Note translation
//
const int numNotes = 58;
const int numCodes = 92;

// MIDI codes for each note
int MidiNum[] = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, // C2..B2  0-11
                 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, // C3..B3  12-23
                 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, // C4..B4  24-35
                 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, // C5..B5  36-47
                 84, 85, 86, 87, 88, 89, 90, 91, 92, 93};        // C6..A6  48-57

//Note code for each keyboard code
unsigned long code_note[] = {13869,13997,13868,13870,13864,13856,13832,13824,14080,13312,13328,12288,
                             5677, 5805, 5676, 5678, 5672, 5664, 5640, 5632, 5888, 5120, 5136, 4096,
                             1024, 0,   38444,38446,38440,38432,38408,38400,38656,37888,37904,36864,
                             33792,32768,40960,57344,57408,59456,22024,22016,22272,21504,21520,20480,
                             17408,16384,
                             9216,8192,5668,4112,37412,36880,22060,22062,22056,22048,22020,
                             49152,50176,53248,54288,54272,55040,54784,54792,54816,54824,54830,54828,
                             40960,41984,45056,46096,46080,46848,46592,46600,46624,46632,46638,46636,46765,46637,
                             17416,17440,17448,17454,17452};  // Alternativas

// Note index in the array of MIDI numbers corresponding to each keyboard code
int noteIndex[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,
                    12,13,14,15,16,17,18,19,20,21,22,23,
                    24,25,26,27,28,29,30,31,32,33,34,35,
                    36,37,38,39,40,41,42,43,44,45,46,47,
                    48,49,
                    12,13,18,24,30,36,38,39,40,41,42,
                    37,36,35,34,33,32,31,30,29,28,27,26,
                    13,12,11,10,9,8,7,6,5,4,3,2,1,0,
                    54,53,52,51,50};      // Alternativas
                                
//
// Mouthpiece
//
boolean blowing = false;  // True if blowing the mouthpiece, False otherwise

//
// Musical Keys
//
const int CKey = 0;          // (Spanish: Tono de Do)
const int EFlatKey = 3;      // (Spanish: Tono de Mi Bemol)
const int BFlatKey = -2;     // (Spanish: Tono de Si Bemol)
int musicalKey = EFlatKey; 

//
// Instrument pitch selection
//
const byte ws_LOW = 0;                // Pitch wheel state LOW, meaning 0V
const byte ws_HIGH = 1;               // Pitch wheel state HIGH, meaning 5V
const byte ws_INIT = 255;             // Pitch wheel state INIT (only used when the instrument is turned on)
const byte secundaryInstrument = 0;   // Indentifies the secundary instrument
const byte principalInstrument = 1;   // Indentifies the principal instrument
byte pitchWheel = ws_INIT;            // Pitch wheel state in a previous time interval
boolean wheelFire = false;            // TRUE if the pith wheel state goes from LOW to HIGH, FALSE otherwise
byte selectedInstrument;              // Current selected instrument:  principal / secundary
boolean pitchSelection;               // TRUE if instrument pitch can be changed (using the Pitch wheel), FALSE otherwise
byte pitch[2];                        // Pitch index sets to each instrument

//
// Analog magnitudes
//
const byte baseVolume = 20;             // Minimum volume level (in the 127 levels scale)
unsigned int volumePinReading;          // From 0 to 1023
unsigned int volume1024;                // From 0 to 1023
unsigned int volumeInertia = 0;         // From 0 to 1023
int volume128;                          // Normalized from baseVolume to 127

unsigned int pitchBendPinReading;       // From 0 to 1023
unsigned int pitchBend1024;             // From 0 to 1023
unsigned int pitchBendInertia = 0;      // From 0 to 1023
int pitchBend64;                        // Normalized from baseVolume to 127 // OJO Recordar que aquí solo es la mitad
bool pitchBendEnabled;


byte busState;
const unsigned long changeInstrumentCode = 64;
bool alterPressed = false;

//
// MIDI constants
//
const byte VS1053_BANK_DEFAULT = 0x00;
const byte VS1053_BANK_DRUMS1  = 0x78;
const byte VS1053_BANK_DRUMS2  = 0x7F;
const byte VS1053_BANK_MELODY  = 0x79;

const byte MIDI_NOTE_ON        = 0x90;
const byte MIDI_NOTE_OFF       = 0x80;
const byte MIDI_CHAN_MSG       = 0xB0;
const byte MIDI_CHAN_BANK      = 0x00;
const byte MIDI_CHAN_VOLUME    = 0x07;
const byte MIDI_CHAN_PROGRAM   = 0xC0;
const byte MIDI_PITCH_BEND     = 0xE0;

SoftwareSerial VS1053_MIDI(0, 2); // TX only, do not use the 'rx' side

void setup() {
  // Disable read/write function in serial port
  UCSR0B &= ~(_BV(TXEN0));  //disable UART TX 
  UCSR0B &= ~(_BV(RXEN0));  //disable UART RX 
  
  // Enable internal pull up on pin 0 (RX)
  DDRD  &= 0b11111110;  // set pin 0 to INPUT direction
  PORTD |= 0b00000001;  // set pin 0 in pull up state
  
  pinMode(pitchSel, OUTPUT);  //
  pinMode(principalInst, OUTPUT);  //
  pinMode(pulseLane0, OUTPUT);
  pinMode(pulseLane1, OUTPUT);
  pinMode(pulseLane2, OUTPUT);
  pinMode(pulseLane3, OUTPUT);
  pinMode(demux, OUTPUT);
  pinMode(controlBank1, INPUT);
  pinMode(controlBank2, INPUT);
  pinMode(CKeyIndicator, OUTPUT);
  pinMode(EFlatKeyIndicator, OUTPUT);
  pinMode(BFlatKeyIndicator, OUTPUT);
  pinMode(matrix0, INPUT);
  pinMode(matrix1, INPUT);
  pinMode(matrix2, INPUT);
  pinMode(matrix3, INPUT);
  pinMode(volumePin, INPUT);
  pinMode(pitchBendPin, INPUT);

  pitchSelection = false;
  digitalWrite(pitchSel, LOW);  //
  
  selectedInstrument = principalInstrument;
  digitalWrite(principalInst, HIGH);  //
  
  VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate'
  
  pinMode(VS1053_RESET, OUTPUT);
  digitalWrite(VS1053_RESET, LOW);
  delay(10);
  digitalWrite(VS1053_RESET, HIGH);
  delay(10);
  
  pitch[principalInstrument] = 0;  // Default primary: Sax
  pitch[secundaryInstrument] = 5;  // Default secundary: Organ
  
  midiSetChannelBank(0, VS1053_BANK_MELODY);
  midiSetInstrument(0, pitchTable[pitch[selectedInstrument]]);
  midiSetChannelVolume(0, 64);
  
//  musicalKey = EFlatKey;
  digitalWrite(CKeyIndicator, LOW);
  digitalWrite(EFlatKeyIndicator, HIGH);
  digitalWrite(BFlatKeyIndicator, LOW);
  
  playingMusic = false;
}

void loop() {
  //
  // Updating instrument board indicators (Board Lights)
  //
  PORTD = (PORTD & 0b00000111)|(pitch[selectedInstrument] << 3);

  if (connectedPins(pulseLane2, controlBank1)) {
    if (!alterPressed) {
      alterPressed = true;
      if (pitchSelection) setPitchSelection(false);
      setSelectedInstrument(secundaryInstrument);  // Activa el instrumento secundario
    }
  } else {
    if (alterPressed) {
      alterPressed = false;
      setSelectedInstrument(principalInstrument);  // Reactiva el instrumento principal
    }
  }
  
  //
  // Reading Control Bank 1
  //
  if (!alterPressed) {
    if (connectedPins(pulseLane0, controlBank1)) { // Activa el modo de selección del timbre para el instrumento secundario
      setPitchSelection(true);
      setSelectedInstrument(secundaryInstrument);
    }
  }

  if (!alterPressed) {
    if (connectedPins(pulseLane1, controlBank1)) { // Activa el modo de selección del timbre para el instrumento primario
      setPitchSelection(true);
      setSelectedInstrument(principalInstrument);
    }
  }
  
  //
  // Reading Control Bank 2
  //

  if (connectedPins(pulseLane0, controlBank2)) { // The performer is blowing the mouthpiece
    blowing = true;
    if (pitchSelection) {
      setPitchSelection(false);
      setSelectedInstrument(principalInstrument);
    }
  } else {
    blowing = false;  // The performer is not blowing the mouthpiece
  }

  if (connectedPins(pulseLane1, controlBank2)) {
    if (musicalKey != CKey) {
      musicalKey = CKey;
      digitalWrite(CKeyIndicator, HIGH);
      digitalWrite(EFlatKeyIndicator, LOW);
      digitalWrite(BFlatKeyIndicator, LOW);
    }
  }

  if (connectedPins(pulseLane2, controlBank2)) {
    if (musicalKey != EFlatKey) {
      musicalKey = EFlatKey;
      digitalWrite(CKeyIndicator, LOW);
      digitalWrite(EFlatKeyIndicator, HIGH);
      digitalWrite(BFlatKeyIndicator, LOW);
    }
  }

  if (connectedPins(pulseLane3, controlBank2)) {
    if (musicalKey != BFlatKey) {
      musicalKey = BFlatKey;
      digitalWrite(CKeyIndicator, LOW);
      digitalWrite(EFlatKeyIndicator, LOW);
      digitalWrite(BFlatKeyIndicator, HIGH);
    }
  }

  //  
  // Reading Analog Magnitudes
  //
  volumePinReading = analogRead(volumePin);
  volume1024 = (volumePinReading + volumeInertia)/2;
  volumeInertia = volume1024;
  volume128 = baseVolume + map(volume1024, 60, 1000, 0, 127 - baseVolume);
  midiSetChannelVolume(0, volume128);

  if (connectedPins(pulseLane3, controlBank1)) {
    pitchBendPinReading = analogRead(pitchBendPin);
    pitchBend1024 = (pitchBendPinReading + pitchBendInertia)/2;
    pitchBendInertia = pitchBend1024;
    pitchBend64 = map(pitchBend1024, 100, 300, 0, 64);
    if (playingMusic) {
      midiPitchBend(0, 64, 64 - pitchBend64);
    }
  }
  
  //
  // Reading the Matrix
  //
  if (keyboardChanged() && !keyboardPreviouslyChanged) {
    keyboardChangeStart = micros();
    keyboardPreviouslyChanged = true;
    }
  if (keyboardPreviouslyChanged && (micros() - keyboardChangeStart >= persistentChangeLength)) {
    keyboardPersistentChange = true;
    keyboardPreviouslyChanged = false;
    }
  if (keyboardPersistentChange) {
    unsigned long code = noteCode(keyboardCurrentState);
    if (code == changeInstrumentCode) {
      if (pitchSelection) {
        pitch[selectedInstrument]++;
        if (pitch[selectedInstrument] == pitchNumber) {
          pitch[selectedInstrument] = 0;
        }
      }
      saveKeyboardState();
    }
    int note = MIDI_Number(code);
    if (note > 0) { //note found; note==0 => key pressing blunder
      saveKeyboardState();
      shutdownNote(previousNote);
      previousNote = note;
    }
    keyboardPersistentChange = false;
  }
  if (!playingMusic && blowing) {
    if (previousNote != 0) {
      playNote(previousNote);
    }
  } 
  if (playingMusic && !blowing) {
    if (previousNote != 0) {
      shutdownNote(previousNote);
    }
  }
}  // end of loop

void playNote(unsigned int note) {
  midiNoteOn(0, note, 127);
  playingMusic = true;
}

void shutdownNote(unsigned int note) {
  midiNoteOff(0, note, 127);
  playingMusic = false;
}

boolean keyboardChanged() { // retorna True si hubo cambio en el estado del teclado
  boolean change = false;
  saveState();
  for (int i = 0; i < 4; i++) {
    //escribir salida para el decodificador
    if (i==0) digitalWrite(pulseLane0, HIGH); else digitalWrite(pulseLane0, LOW);
    if (i==1) digitalWrite(pulseLane1, HIGH); else digitalWrite(pulseLane1, LOW);
    if (i==2) digitalWrite(pulseLane2, HIGH); else digitalWrite(pulseLane2, LOW);
    if (i==3) digitalWrite(pulseLane3, HIGH); else digitalWrite(pulseLane3, LOW);
    keyboardCurrentState[i] = 0b00001111 & PINC;                     //read columns input
    if (keyboardCurrentState[i] != keyboardPreviousState[i]) {       //compare with previous reading
      change = true;  // local variable, reset to false in each keyboard reading
    }
  }
  restoreState();
  return change;
}

unsigned long noteCode(byte keyboard[]) { // Concatenate the 4 keyboard reading
  unsigned long code = 0;
  //form the note code concatenating las the 4 keyboard reading
  for (int i = 3; i >= 0; i--)  {
    code = (code << 4) | keyboard[i];  //shift 4 places to the left
  }                                      //bitwise OR with new reading
  return code;
}

int MIDI_Number(unsigned long code) { // Find the note MIDI number from the keyboard code
 // look up the corresponding note
 for (int i = 0; i < numCodes; i++) {
  if (code == code_note[i]) {
    return MidiNum[noteIndex[i] + musicalKey];
    }
  }
  return 0;
}

void saveKeyboardState() { // Save the current keyboard state
  //set previous reading
  for (int i = 0; i < 4; i++) {
      keyboardPreviousState[i] = keyboardCurrentState[i];  
  }
}

void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // Actually, instruments code start with 0, not 1
  if (inst > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);  
  VS1053_MIDI.write(inst);
}

void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) vol = 127;
  if (vol < 0) vol = 0;
  
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write((uint8_t)MIDI_CHAN_BANK);
  VS1053_MIDI.write(bank);
}

void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;
  
  VS1053_MIDI.write(MIDI_NOTE_ON | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;
  
  VS1053_MIDI.write(MIDI_NOTE_OFF | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiPitchBend(uint8_t chan, uint8_t msb, uint8_t lsb) {
  if (chan > 15) return;
  if (msb > 127) msb = 127;
  if (lsb > 127) lsb = 127;
  if (lsb < 0) lsb = 0;
  
  VS1053_MIDI.write(MIDI_PITCH_BEND | chan);
  VS1053_MIDI.write(msb);
  VS1053_MIDI.write(lsb);
}

/*
 * Saves and clears state of digital pins D3-D6 (bus lines) and disables demultiplexer
 */
void saveState() {
  busState = PORTD & 0b01111000;   // Save registers
  PORTD &= 0b00000111;             // Clear registers
  PORTD |= 0b10000000;             // Disables demultiplexer
}

/*
 * Restores saved state of digital pins D3-D6 (bus lines) and enables demultiplexer
 */
void restoreState() {
  PORTD = (PORTD & 0b00000111)|busState;
  PORTD &= 0b01111111;                      // Enables demultiplexer
}

/*
 * Clears state of digital pins D3-D6 (bus lines) and disables demultiplexer
 */
void clearState() {
  PORTD = PORTD & 0b10000111;
}

bool connectedPins(byte emiter, byte receiver) {
  busState = PORTD & 0b01111000;              // Save registers
  PORTD &= 0b10000111;                        // Clear registers
  PORTD |= 0b10000000;                        // Disables demultiplexer
  delayMicroseconds(1);
  digitalWrite(emiter, HIGH);
  delayMicroseconds(1);
  bool result = digitalRead(receiver) == HIGH;
  PORTD = (PORTD & 0b00000111)|busState;      // Restores state
  PORTD &= 0b01111111;                        // Enables demultiplexer
  return result;
}

void setPitchSelection(bool val){
  pitchSelection = val;
  digitalWrite(pitchSel, val);
}

void setSelectedInstrument(byte val) {
  if (selectedInstrument != val) {
    selectedInstrument = val;
    digitalWrite(principalInst, val);  //
    midiSetInstrument(0, pitchTable[pitch[selectedInstrument]]);
  }
}
