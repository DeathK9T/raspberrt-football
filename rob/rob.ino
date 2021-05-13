#define mot_1_forward  0
#define mot_1_back     1
#define mot_1_pwr     3

#define mot_2_forward  2
#define mot_2_back     3
#define mot_2_pwr      6

#define mot_3_forward  4
#define mot_3_back     5
#define mot_3_pwr      9

#define mot_4_forward  6
#define mot_4_back     7
#define mot_4_pwr      12


#define T 10000
#define t_sh1 1000
#define t_sh2 1000
#define t_sh3 1000
#define t_sh4 1000

boolean SH1 = true,
        SH2 = true,
        SH3 = true,
        SH4 = true;

int dir = 5;

long t00 = 0,
     t01 = 0;

void movel(int dir)
{
  switch(dir)
  {
    case 8:
    {
      digitalWrite(mot_1_forward,HIGH);
      digitalWrite(mot_1_back,LOW);
      
      digitalWrite(mot_2_forward,HIGH);
      digitalWrite(mot_2_back,LOW);
      
      digitalWrite(mot_3_forward,LOW);
      digitalWrite(mot_3_back,HIGH);
      
      digitalWrite(mot_4_forward,LOW);
      digitalWrite(mot_4_back,HIGH);      
      
      break;
    }
    
    case 2:
    {
      digitalWrite(mot_1_forward,LOW);
      digitalWrite(mot_1_back,HIGH);
      
      digitalWrite(mot_2_forward,LOW);
      digitalWrite(mot_2_back,HIGH);
      
      digitalWrite(mot_3_forward,HIGH);
      digitalWrite(mot_3_back,LOW);
      
      digitalWrite(mot_4_forward,HIGH);
      digitalWrite(mot_4_back,LOW);      
      
      break;
    }
    
    case 4:
    {
      digitalWrite(mot_1_forward,LOW);
      digitalWrite(mot_1_back,HIGH);
      
      digitalWrite(mot_2_forward,HIGH);
      digitalWrite(mot_2_back,LOW);
      
      digitalWrite(mot_3_forward,HIGH);
      digitalWrite(mot_3_back,LOW);
      
      digitalWrite(mot_4_forward,LOW);
      digitalWrite(mot_4_back,HIGH);      
      
      break;
    }
    
    case 6:
    {
      digitalWrite(mot_1_forward,HIGH);
      digitalWrite(mot_1_back,LOW);
      
      digitalWrite(mot_2_forward,LOW);
      digitalWrite(mot_2_back,HIGH);
      
      digitalWrite(mot_3_forward,LOW);
      digitalWrite(mot_3_back,HIGH);
      
      digitalWrite(mot_4_forward,HIGH);
      digitalWrite(mot_4_back,LOW);        
      
      break;
    }
    
    case 5:
    {
      digitalWrite(mot_1_forward,HIGH);
      digitalWrite(mot_1_back,HIGH);
      
      digitalWrite(mot_2_forward,HIGH);
      digitalWrite(mot_2_back,HIGH);
      
      digitalWrite(mot_3_forward,HIGH);
      digitalWrite(mot_3_back,HIGH);
      
      digitalWrite(mot_4_forward,HIGH);
      digitalWrite(mot_4_back,HIGH);      
      
      break;
    }
    case 7:
    {
      digitalWrite(mot_1_forward,LOW);
      digitalWrite(mot_1_back,HIGH);
      
      digitalWrite(mot_2_forward,LOW);
      digitalWrite(mot_2_back,HIGH);
      
      digitalWrite(mot_3_forward,LOW);
      digitalWrite(mot_3_back,HIGH);
      
      digitalWrite(mot_4_forward,LOW);
      digitalWrite(mot_4_back,HIGH);      
      
      break;
    }
    case 9:
    {
      digitalWrite(mot_1_forward,HIGH);
      digitalWrite(mot_1_back,LOW);
      
      digitalWrite(mot_2_forward,HIGH);
      digitalWrite(mot_2_back,LOW);
      
      digitalWrite(mot_3_forward,HIGH);
      digitalWrite(mot_3_back,LOW);
      
      digitalWrite(mot_4_forward,HIGH);
      digitalWrite(mot_4_back,LOW);      
      
      break;
    }
    default:
    {
      break;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  
  //pinMode(A0, INPUT);
  //pinMode(A3, INPUT);
  
  pinMode(mot_1_forward,OUTPUT);
  pinMode(mot_1_back,OUTPUT);
  pinMode(mot_1_pwr,OUTPUT);
  
  pinMode(mot_2_forward,OUTPUT);
  pinMode(mot_2_back,OUTPUT);
  pinMode(mot_2_pwr,OUTPUT);
  
  pinMode(mot_3_forward,OUTPUT);
  pinMode(mot_3_back,OUTPUT);
  pinMode(mot_3_pwr,OUTPUT);
  
  pinMode(mot_4_forward,OUTPUT);
  pinMode(mot_4_back,OUTPUT);
  pinMode(mot_4_pwr,OUTPUT);
}

void loop()
{
  movel(dir);
  SH1 = true;
  SH2 = true;
  SH3 = true;
  SH4 = true;
  t00 = micros();
  digitalWrite(mot_1_pwr,HIGH);
  digitalWrite(mot_2_pwr,HIGH);
  digitalWrite(mot_3_pwr,HIGH);
  digitalWrite(mot_4_pwr,HIGH);
  do
  {
    t01 = micros();
    if(SH1 && ((t01 - t00) > t_sh1))
    {
      SH1 = false;
      digitalWrite(mot_1_pwr,LOW);
    }
    if(SH2 && ((t01 - t00) > t_sh2))
    {
      SH2 = false;
      digitalWrite(mot_2_pwr,LOW);
    }
    if(SH3 && ((t01 - t00) > t_sh3))
    {
      SH3 = false;
      digitalWrite(mot_3_pwr,LOW);
    }
    if(SH4 && ((t01 - t00) > t_sh4))
    {
      SH4 = false;
      digitalWrite(mot_4_pwr,LOW);
    }
    if(Serial.available()>0)
    {
      dir = (int)Serial.read();
      Serial.print((char)(analogRead(A0) >> 2));
      Serial.print((char)(analogRead(A3) >> 2));
    }
  }while(t01 - t00 < T);
}
  
