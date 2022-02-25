  #include <Wire.h>
  #include <ArduinoHardware.h>
  #include <ros.h>
  #include <geometry_msgs/Twist.h>
  #include <std_msgs/String.h>
  #include <tf/transform_broadcaster.h>
  #include <tf/tf.h>
 
  #define pwm_c 5
  #define EN_L 8
  #define IN1_L 32
  #define IN2_L 33
  
  #define EN_R 9
  #define IN1_R 34
  #define IN2_R 35
  
  #define EN_R_b 10
  #define IN1_R_b 36
  #define IN2_R_b 37
   
  
  #define EN_L_b 11
  #define IN1_L_b 38
  #define IN2_L_b 39
  
  double w_r = 0, w_l = 0;
  
  
  double wheel_rad = 0.0325, wheel_sep = 0.295;
  
  geometry_msgs::TransformStamped t;
  tf::TransformBroadcaster broadcaster;
  char base_link[] = "/base_link";
  char odom[] = "/odom";
  double  xytCounts[2];
  ros::NodeHandle nh;
  std_msgs::String str_msg;
  ros::Publisher chatter("chatter", &str_msg);
  int lowSpeed = 200;
  int highSpeed = 50;
  double speed_ang = 0, speed_lin , speed_ylin, angle_c, x_c, y_c;
  
  float w1=0.0,w2=0.0,w3=0.0,w4=0.0,wsum=0.0,lx=0.15,ly=0.25,rad=1.0;
  int s1,s2,s3,s4;
  
  void messageCb( const geometry_msgs::Twist& msg) {
  
  angle_c = msg.angular.z;
  x_c     = msg.linear.x;
  y_c     = msg.linear.y;
  wsum=angle_c*(lx+ly);
  w1=10/rad*(x_c-y_c-wsum);
  w2=10/rad*(x_c+y_c+wsum);
  w3=10/rad*(x_c+y_c-wsum);
  w4=10/rad*(x_c-y_c+wsum);
  
  }
  
  
  ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", &messageCb );
  
  void setup() {
    Wire.begin();
    pinMode(pwm_c,OUTPUT);
    digitalWrite(pwm_c,0);
    Serial1.begin(57600);
    nh.initNode();
    nh.advertise(chatter);
    nh.subscribe(sub);
    broadcaster.init(nh);
  
  }
  
  
  void loop() {
  s1=w1*1;
  s2=w2*1;
  s3=w3*1;
  s4=w4*1;
//  analogWrite(pwm_c, s1);
//  Serial1.println(s1);
//  Serial1.println(s2);
//  Serial1.println(s3);
//  Serial1.println(s4);
  
    int16_t omega1,omega2,omega3,omega4;
    byte a, b;
    Wire.requestFrom(4, 2);
    a = Wire.read();
    b = Wire.read();
    omega1 = a;
    omega1 = (omega1 << 8) | b;
    //delay(100);
    
     Wire.requestFrom(5, 2);
    a = Wire.read();
    b = Wire.read();
    omega2 = a;
    omega2 = (omega2 << 8) | b;
    //delay(100);
    
     Wire.requestFrom(6, 2);
    a = Wire.read();
    b = Wire.read();
    omega3 = a;
    omega3 = (omega3 << 8) | b;
    //delay(100);
    
     Wire.requestFrom(7, 2);
    a = Wire.read();
    b = Wire.read();
    omega4 = a;
    omega4 = (omega4 << 8) | b;
    //delay(100);
    
    
    
  
  
    Wire.beginTransmission(4); 
    Wire.write(s1); 
    Wire.endTransmission();
    
    Wire.beginTransmission(55); 
    Wire.write(s2);
    Wire.endTransmission();
    
    Wire.beginTransmission(56); 
    Wire.write(s3);   
    Wire.endTransmission();
    
    Wire.beginTransmission(57); 
    Wire.write(s4); 
    Wire.endTransmission();
 
  
    xytCounts[0] = (omega1 + omega2 + omega3 + omega4) / 4;
    xytCounts[1] = (0 - omega1 + omega2 + omega3 - omega4) / 4;
    xytCounts[2] = (0 - omega1 + omega2 - omega3 + omega4) / 4;
  
    t.header.frame_id = odom;
    t.child_frame_id = base_link;
    
    t.transform.translation.x = (xytCounts[0]);
    t.transform.translation.y = (xytCounts[1]);
    
    t.transform.rotation = tf::createQuaternionFromYaw(xytCounts[2]);
    t.header.stamp = nh.now();
    
    broadcaster.sendTransform(t);
  
   
    nh.spinOnce();
    delay(10);
  
  }
