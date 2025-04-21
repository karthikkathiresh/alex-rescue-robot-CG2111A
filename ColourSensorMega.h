/* 
refer to
Dr. G. Paavai A., Shreeram S. N., Adithya A.  Color Detection using K-Nearest Neighbour (November 2024) IJARSCT
to understand the colour detection technique
 */


#define s0 30       //Module pins wiring
#define s1 32
#define s2 34
#define s3 36
#define out 38

// #define led 7
#define SAMPLES 3
#define K 5
#define THRESHOLD 1000


int data=0;        //This is where we're going to stock our values

typedef enum {
  RED = 0,
  GREEN = 1,
  NEITHER = 2,
} TCol;

struct dist
{
  unsigned long val;
  TCol col;
};



static int green_samples[SAMPLES][3] = {
  {89,54,68},
  {89,54,68}, 
  {88,52,66}
};

static int red_samples[SAMPLES][3] = {
  {81,157,127},
  {79,156,126},
  {84,159,129}
};

unsigned long euc_dist(int A[3], int B[3]) {  //might need to change to long
  return sq(A[0]- B[0]) + sq(A[1]- B[1]) + sq(A[2]- B[2]);
}

int colour(int r, int g, int b) {
  int input[3] = {r, g, b};
  struct dist n_dist[SAMPLES*2]; // n is for neighbour
  for (int i = 0; i < SAMPLES; i++) {
    int sample[3] = {green_samples[i][0], green_samples[i][1], green_samples[i][2]};
    n_dist[i].col = GREEN;
    n_dist[i].val = euc_dist(input, sample);
  }
    for (int i = 0; i < SAMPLES; i++) {
    int sample[3] = {red_samples[i][0], red_samples[i][1], red_samples[i][2]};
    n_dist[i].col = RED;
    n_dist[i + SAMPLES].val = euc_dist(input, sample);
  }
  // sort n_dist
  bool sorted = false;
  while (!sorted) {
    sorted = true;
    for (int i = 0; i < SAMPLES*2; i++) {
      if (n_dist[i].val > n_dist[i+1].val) {
        struct dist temp = n_dist[i+1];
        n_dist[i+1] = n_dist[i];
        n_dist[i] = temp; 
        sorted = false;
      }
    }
  }

  // the actual k-nearest neighbours part
  int votes[3] = {0};
  for (int i = 0; i < K; i++)
  {
    if (n_dist[i].val < THRESHOLD)
      votes[n_dist[i].col] += 1;
    votes[2] += 1;
  }

  return max(votes[0], max(votes[1], votes[2]));
}

void colourSetup() 
{
   pinMode(s0,OUTPUT);    //pin modes
   pinMode(s1,OUTPUT);
   pinMode(s2,OUTPUT);
   pinMode(s3,OUTPUT);
  //  pinMode(led,OUTPUT);
   pinMode(out,INPUT);

   Serial.begin(9600);   //intialize the serial monitor baud rate
   
   digitalWrite(s0,HIGH);  //Putting S0/S1 on HIGH/HIGH levels means the output frequency scalling is at 100%  (recommended)
   digitalWrite(s1,HIGH); //LOW/LOW is off HIGH/LOW is 20% and  LOW/HIGH is  2%
  //  digitalWrite(led, HIGH);
   
}

int colourLoop()                  //Every 2s we select  a photodiodes set and read its data
{

   digitalWrite(s2,LOW);        //S2/S3  levels define which set of photodiodes we are using LOW/LOW is for RED LOW/HIGH  is for Blue and HIGH/HIGH is for green
   digitalWrite(s3,LOW);
   Serial.print("Red  value= "); 
   int red = GetData();                   //Executing GetData function to  get the value

   digitalWrite(s2,LOW);
   digitalWrite(s3,HIGH);
   Serial.print("Blue  value= ");
   int blue = GetData();

   digitalWrite(s2,HIGH);
   digitalWrite(s3,HIGH);
   Serial.print("Green value= ");
   int green = GetData();
 
  return colour(red, green, blue); // red is 0, green is 1, neither is 2 (HAS NOT BEEN TRAINED)
}

int GetData(){
   data=pulseIn(out,LOW);       //here  we wait until "out" go LOW, we start measuring the duration and stops when "out"  is HIGH again
   Serial.print(data);          //it's a time duration measured,  which is related to frequency as the sensor gives a frequency depending on the color
   Serial.print("\ ");          //The higher the frequency the lower the duration
   delay(20);
   return data;
}
