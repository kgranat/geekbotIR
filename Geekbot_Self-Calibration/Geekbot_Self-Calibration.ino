#define LEFT_SERVO_PIN 10
#define RIGHT_SERVO_PIN 11
#define LEFT_ENCODER_PIN 1
#define RIGHT_ENCODER_PIN 2

#define CORRECTION_DELAY 5000
#define SATISFIED_DELAY 15000

#include <Servo.h>

#include <Wire.h>                 //Load for I2C
#include <LiquidCrystal_I2C.h>    //Load the LiquidCrystal I2C Library for the LCD Display
LiquidCrystal_I2C lcd(0x27, 16, 2);  //I2C LCD Screen

Servo servo_left, servo_right;

bool testingCW = true;

bool cal_left_cw = false;
bool cal_left_ccw = false;
bool cal_right_cw = false;
bool cal_right_ccw = false;

//PWMs for Left and Right servos in both CW and CCW directions.
//Set to ideal conditions
int CW_left = 1400;
int CW_right = 1400;
int CCW_left = 1600;
int CCW_right = 1600;

unsigned long last_integration = millis();
unsigned long last_speed_adjustment_left = millis();
unsigned long last_speed_adjustment_right = millis();

const int ENCODER_VALUE_THRESHOLD = 250; //ADC input value for high/low signal change
const int encoderCounts_per_revolution = 64; //Number of slices on wheel encoder
bool _rightEncoderRising = true;    //state of encoder counting
bool _rightEncoderFalling = false;  //state of encoder counting
bool _leftEncoderRising = true;     //state of encoder counting
bool _leftEncoderFalling = false;   //state of encoder counting
int _rightEncoderCount = 0; //number of encoder ticks per interval during command
int _leftEncoderCount = 0; //number of encoder ticks per interval during command

double desired_RPM = 23; //TODO: Refactor to desired_ticks
double rpm_left = desired_RPM; //TODO: Refactor
double rpm_right = desired_RPM; //TODO: Refactor

void setup()
{
  servo_left.attach(LEFT_SERVO_PIN);
  servo_right.attach(RIGHT_SERVO_PIN);

  Serial.begin( 38400 );

  lcd.begin();
  lcd.backlight();

}

void loop()
{
  check_encoders(); //Always update current encoder state
  
  if ( testingCW == true )
  {
    if ( !cal_left_cw ) servo_left.writeMicroseconds( CW_left );
    if ( !cal_right_cw ) servo_right.writeMicroseconds( CW_right );

    if ( last_integration + 1000 < millis() )
    {
      last_integration = millis();
      
      if ( !cal_left_cw ) rpm_left = ( 0.9 * rpm_left ) + ( 0.1 * (_leftEncoderCount) );
      if ( !cal_right_cw ) rpm_right = ( 0.9 * rpm_right ) + ( 0.1 * (_rightEncoderCount ) );
      _leftEncoderCount = 0;
      _rightEncoderCount = 0;

      lcd.clear();
      lcd.print( "L:" );
      lcd.print( rpm_left );
      lcd.print( " " );
      lcd.setCursor(8, 0);
      lcd.print( "R:" );
      lcd.print( rpm_right );
      lcd.setCursor(0, 1);
      lcd.print( CW_left );
      lcd.setCursor(8, 1);
      lcd.print( CW_right );

      lcd.setCursor(5, 1);
      if ( !cal_left_cw ) lcd.print( (millis() - last_speed_adjustment_left) / 1000 );

      lcd.setCursor(13, 1);
      if ( !cal_right_cw ) lcd.print( (millis() - last_speed_adjustment_right) / 1000 );
//LEFT
      if ( last_speed_adjustment_left + CORRECTION_DELAY < millis() && round(rpm_left) != desired_RPM && !cal_left_cw )
      {
        last_speed_adjustment_left = millis();
        if ( rpm_left < desired_RPM )
        {
          --CW_left;
        }
        else
        {
          ++CW_left;
        }
        servo_left.writeMicroseconds( CW_left );
      }
      if ( last_speed_adjustment_left + SATISFIED_DELAY < millis() && round(rpm_left) == desired_RPM )
      {
        cal_left_cw = true;
        servo_left.writeMicroseconds( 1500 ); //Stop servo
      }
//RIGHT
      if ( last_speed_adjustment_right + CORRECTION_DELAY < millis() && round(rpm_right) != desired_RPM && !cal_right_cw )
      {
        last_speed_adjustment_right = millis();
        if ( rpm_right < desired_RPM )
        {
          --CW_right;
        }
        else
        {
          ++CW_right;
        }
        servo_right.writeMicroseconds( CW_right );
      }
      if ( last_speed_adjustment_right + SATISFIED_DELAY < millis() && round(rpm_right) == desired_RPM )
      {
        cal_right_cw = true;
        servo_right.writeMicroseconds( 1500 ); //Stop servo
      }
//DONE
      if ( cal_left_cw == true && cal_right_cw == true )
      {
        testingCW = false;
      }
    }
  }
  else if ( cal_left_ccw == false || cal_right_ccw == false )
  {
    if ( !cal_left_ccw ) servo_left.writeMicroseconds( CCW_left );
    if ( !cal_right_ccw ) servo_right.writeMicroseconds( CCW_right );

    if ( last_integration + 1000 < millis() )
    {
      last_integration = millis();
      
      if ( !cal_left_ccw ) rpm_left = ( 0.9 * rpm_left ) + ( 0.1 * (_leftEncoderCount) );
      if ( !cal_right_ccw ) rpm_right = ( 0.9 * rpm_right ) + ( 0.1 * (_rightEncoderCount ) );
      _leftEncoderCount = 0;
      _rightEncoderCount = 0;

      lcd.clear();
      lcd.print( "L:" );
      lcd.print( rpm_left );
      lcd.print( " " );
      lcd.setCursor(8, 0);
      lcd.print( "R:" );
      lcd.print( rpm_right );
      lcd.setCursor(0, 1);
      lcd.print( CCW_left );
      lcd.setCursor(8, 1);
      lcd.print( CCW_right );

      lcd.setCursor(5, 1);
      if ( !cal_left_ccw ) lcd.print( (millis() - last_speed_adjustment_left) / 1000 );

      lcd.setCursor(13, 1);
      if ( !cal_right_ccw ) lcd.print( (millis() - last_speed_adjustment_right) / 1000 );
//LEFT
      if ( last_speed_adjustment_left + CORRECTION_DELAY < millis() && round(rpm_left) != desired_RPM && !cal_left_ccw )
      {
        last_speed_adjustment_left = millis();
        if ( rpm_left > desired_RPM )
        {
          --CCW_left;
        }
        else
        {
          ++CCW_left;
        }
        servo_left.writeMicroseconds( CCW_left );
      }
      if ( last_speed_adjustment_left + SATISFIED_DELAY < millis() && round(rpm_left) == desired_RPM )
      {
        cal_left_ccw = true;
        servo_left.writeMicroseconds( 1500 ); //Stop servo
      }
//RIGHT
      if ( last_speed_adjustment_right + CORRECTION_DELAY < millis() && round(rpm_right) != desired_RPM && !cal_right_ccw )
      {
        last_speed_adjustment_right = millis();
        if ( rpm_right > desired_RPM )
        {
          --CCW_right;
        }
        else
        {
          ++CCW_right;
        }
        servo_right.writeMicroseconds( CCW_right );
      }
      if ( last_speed_adjustment_right + SATISFIED_DELAY < millis() && round(rpm_right) == desired_RPM )
      {
        cal_right_ccw = true;
        servo_right.writeMicroseconds( 1500 ); //Stop servo
      }
//DONE
      if ( cal_left_ccw == true && cal_right_ccw == true )
      {
        testingCW = false;
      }
    }
  }
  else
  {
    lcd.clear();
    lcd.print( "L:" );
    lcd.print( CW_left );
    lcd.print( " " );
    lcd.setCursor(8, 0);
    lcd.print( "R:" );
    lcd.print( CW_right );
    lcd.setCursor(0, 1);
    lcd.print( CCW_left );
    lcd.setCursor(8, 1);
    lcd.print( CCW_right );
    while( 1 );
  }
  
}

void check_encoders()
{
  int rightEncoderValue = analogRead(RIGHT_ENCODER_PIN); // get encoder value
  int leftEncoderValue = analogRead(LEFT_ENCODER_PIN);

  //Catch falling edge ( white stripe ) for right and left wheel encoders
  if (_rightEncoderFalling && rightEncoderValue < ENCODER_VALUE_THRESHOLD)
  {
    ++_rightEncoderCount;
    _rightEncoderRising = true;
    _rightEncoderFalling = false;
  }
  if (_leftEncoderFalling && leftEncoderValue < ENCODER_VALUE_THRESHOLD)
  {
    ++_leftEncoderCount;
    _leftEncoderRising = true;
    _leftEncoderFalling = false;
  }

  //Catch rising edge ( black stripe ) for right and left wheel encoders
  if (_rightEncoderRising && rightEncoderValue > ENCODER_VALUE_THRESHOLD) 
  {
    ++_rightEncoderCount;
    _rightEncoderRising = false;
    _rightEncoderFalling = true;
  } 
  if (_leftEncoderRising && leftEncoderValue > ENCODER_VALUE_THRESHOLD) 
  {
    ++_leftEncoderCount;
    _leftEncoderRising = false;
    _leftEncoderFalling = true;
  }
}

