/*

  BPM.ino
  Version 0.05 24.09.2020
  (c) 2020 by Wolfgang Schuster

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include <U8x8lib.h>
#include <TimerOne.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

unsigned int zaehler = 0;
unsigned int oldzaehler = 0;
char* tmp = " ";
unsigned int oldtime = 0;
unsigned int newtime = 0;
bool istrigger = true;
bool inputmode = true;
int bpm = 120;
int poti = 0;
int potialt = 0;
int feldzaehler = 0;
int werte[20];

void setup(void)
{
  Serial.begin(9600);
  pinMode( 2, INPUT_PULLUP);
  pinMode( 3, INPUT_PULLUP);
  pinMode( 4, OUTPUT);
//  pinMode(A0, INPUT_PULLUP);
  u8x8.begin();
  u8x8.setPowerSave(0);
  attachInterrupt(0, tick, RISING);
  attachInterrupt(1, functionmode, RISING);
  Timer1.initialize(500000);
  Timer1.attachInterrupt(maketick);
  u8x8.setFont(u8x8_font_profont29_2x3_r);
  for(int i=0;i<20;i++)
    werte[i]=analogRead(A0);
}

void loop(void)
{
  potialt=poti;
  werte[feldzaehler]=analogRead(A0);
  feldzaehler++;
  if(feldzaehler>19)
    feldzaehler=0;
  poti = 0;
  for(int i=0;i<20;i++)
    poti = poti + werte[i];
  poti = poti/80;
    
//  if(istrigger==true) {
//    u8x8.clearLine(4);
  istrigger=false;

  u8x8.setFont(u8x8_font_open_iconic_play_2x2);
  u8x8.setCursor(14, 0);
  if(inputmode==true)
    u8x8.print(char(76));
  else
    u8x8.print(char(69));
 
  u8x8.setFont(u8x8_font_profont29_2x3_r);
  u8x8.setCursor(0, 0);
  u8x8.print("BPM");
  
  if(inputmode==true) {
    u8x8.setCursor(1, 4);
    if(60000/(newtime-oldtime)<10)
    {
      u8x8.print("  ");
      u8x8.setCursor(5, 4);
    }
    else if(60000/(newtime-oldtime)<100)
    {
      u8x8.print(" ");
      u8x8.setCursor(3, 4);
    }
    u8x8.print(60000/(newtime-oldtime));
  }
  else {
    u8x8.setCursor(1, 4);
    if(poti<10)
    {
      u8x8.print("  ");
      u8x8.setCursor(5, 4);
    }
    else if(poti<100)
    {
      u8x8.print(" ");
      u8x8.setCursor(3, 4);
    }
    u8x8.print(poti);
  }
}

void functionmode() {
  istrigger = true;
  if(inputmode==true) {
    inputmode=false;
  }
  else {
    inputmode=true;
  }
}

void maketick() {
  if(inputmode==false) {
    istrigger = true;
    tick();
    if(potialt!=poti) {
      Timer1.setPeriod(60000000/(poti+1));
    }
  }
}

void tick() {
  istrigger = true;
  oldtime = newtime;
  newtime = millis();
  oldzaehler=zaehler;
  zaehler++;
  digitalWrite(4, HIGH);
  delay(20);
  digitalWrite(4,LOW);  
}
