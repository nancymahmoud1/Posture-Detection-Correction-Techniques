# **Posture Detection and Correction Using IMUs**

## **Overview**
This project addresses the growing concern of musculoskeletal disorders caused by poor posture. It integrates **Inertial Measurement Units (IMUs)** with a **mobile application** to monitor and correct user posture in real-time, providing a cost-effective and accessible solution for posture-related health issues.

## **Motivation**
According to the World Health Organization (WHO), musculoskeletal disorders are a leading contributor to disability worldwide, with poor posture being a significant risk factor. This system aims to reduce posture-related problems by combining real-time monitoring and personalized corrective feedback.

---

## **Features**
- **Wearable Hardware Design**: 
  - Sensors positioned on critical zones (shoulders and spine).
  - Comfortable and user-friendly wearable base to be integrated in PCB.
  - <img src="https://github.com/user-attachments/assets/6ff4a635-60b1-483b-b1a0-27293f217ad5" alt="image" width="350" height="450">

- **Real-Time Posture Monitoring**:
  - IMUs track pitch and roll angles to detect posture misalignments.
  - Immediate vibrotactile feedback alerts users to maintain correct posture.
- **Mobile Application**:
  - User-friendly interface for tracking posture trends.
  - Personalized exercise recommendations with instructional videos.
  - Gamified features to encourage consistent usage.
---

## **System Components**
### **Hardware**
- **IMUs**: Four MPU6050 sensors placed at:
  - Right and left shoulders.
  - Upper and lower back.
- **Feedback Mechanism**:
  - Vibration motor to notify users of posture deviations.
- **Microcontrollers**: Two ESP32 units for IMU data processing and wireless communication.
- <img src="https://github.com/user-attachments/assets/8bf8577a-60f7-4c8b-aeaf-d4cec705584c" alt="image" width="400" height="300">

### **Software**
- **Postural Data Monitoring**:
  - Tracks user-specific trends to highlight recurring issues.
- **Interactive Feedback**:
  - Suggests corrective exercises and provides educational videos.
- **User Authentication**:
  - Enables personalized tracking and progress monitoring.
  - <img src="https://github.com/user-attachments/assets/9f2bcc98-f1bc-4032-8695-bb3062f8b99a" alt="image" width="400" height="300"> <img src="https://github.com/user-attachments/assets/3b9079d1-5a65-4707-9351-9e6ee336646f" alt="image" width="400" height="300"> 
---

## **How It Works**
1. **Data Collection**: IMUs monitor body orientation and detect posture deviations.
2. **Real-Time Feedback**: Vibrotactile motors alert users if misalignments persist beyond thresholds.
3. **Data Visualization**: The mobile app displays posture trends and suggests exercises.

---

## **Results**
- Successfully detects postural deviations using pitch and roll angles.
- Real-time alerts with a tolerance threshold of ±10 degrees.
- Improved user compliance through interactive feedback and personalized exercises.

---

## **Future Work**
- Incorporate machine learning for enhanced feedback.
- Expand to detect subtle posture deviations.
- Implement cloud-based analytics for long-term trend monitoring.
- Explore applications in sports medicine and occupational therapy.

---

## **Acknowledgments**
Special thanks to **Dr. Aliaa Rehan** for her guidance and **Eng. Nouran Mostafa** for technical assistance. Their support was instrumental in the success of this project.

- I would also like to express my gratitude to my teammates:
  - Nariman Ahmed
  - Biatriss Benyamin
  - Ayat Tarek
  - Saif Mohammed

for their hard work, dedication, and invaluable contributions throughout the development of this project.

---

## **Our Presentation**
[Team4_Presentation.pptx](https://github.com/user-attachments/files/18545541/Team4_Presentation.pptx)


---
