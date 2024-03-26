[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/2TmiRqwI)
# final-project-skeleton

    * Team Name: Byte-cycle
    * Team Members: Maya Huizar and Chris Connolly
    * Github Repository URL: https://github.com/ese3500/final-project-byte-cyclle
    * Github Pages Website URL: [for final submission]
    * Description of hardware: (embedded hardware, laptop, etc) 

## Final Project Proposal

### 1. Abstract

In a few sentences, describe your final project. This abstract will be used as the description in the evaluation survey forms.

Motor vehicle accidents are the leading cause of death amongst cyclists. Our final project is a whole bike safety system equipped with dynamic lights that adjust based on time of day and accelerometer data to detect braking and activate brake lights.

### 2. Motivation

What is the problem that you are trying to solve? Why is this project interesting? What is the intended purpose?

Over 130,000 cyclists are injured every year in the United States. To mitigate the risks of sharing the road with vechiles oftentimes moving at higher speeds, we plan to make a device that increases cyclist safety by increasing visibility. Moreover, the device will be able to measure if the cyclists are slowing down or stopping and will activate brake lights to communicate these actions on the road. 

### 3. Goals

These are to help guide and direct your progress.

By the end of the project, we would like to have brake lights that dynamically adjust based on accelerater data and photosensor data to both reduce power and ensure biker safety.

We would also like a ultrasonic or other sensor system to alert the biker when a car or other large object is very close behind, for increased perception and awareness of surroundings when riding.

If time is available, we would also like to add a display for the rider to show useful information, such as battery life, speed, time riding, and the distance to the nearest car behind them.

### 4. Software Requirements Specification (SRS)

Formulate key software requirements here.

The software most importantly should take in the accelerometer data using serial communication and based on the data, decides whether or not to activate LEDs. An ADC should also be programmed for the photo resistor. Moreover, a pin change interupt should monitor the signal coming from a power management IC that detects voltage sag of the battery. Serial communication to communicate to RTC chip. Also serial communication to display all necessary information to a display. The software should also 3d render ultrasonic data to help visualize distance to cars behind.

### 5. Hardware Requirements Specification (HRS)

Formulate key hardware requirements here.

We will need a low-power usage and small form factor microcontroller to run most of our code, as well as the following sensors:

- ultrasonic sensor (waterproof / enclosed)
- photosensor (for daylight)
- real time clock chip (for current time/date + duration of trip)
- Bright LED or light for brake light
- LED/light to attract attention of user when car is near
- accelerometer (for deceleration data for breaking)
- display (for displaying accelerometer data + )
- power management/voltage supervisor circuit

### 6. MVP Demo

What do you expect to accomplish by the first milestone?

By the first milestone, we want to have the core functionality, that is, the lights turning on due to a rapid deceleration, and low power mode using the photosensor.

### 7. Final Demo

What do you expect to achieve by the final demonstration or after milestone 1?

By the final demonstration, we want to have all features that we claimed ready and working.

### 8. Methodology

What is your approach to the problem?

Our solution is a whole bike safety system equipped with dynamic lights that adjust based on time of day and accelerometer data to detect braking and activate brake lights. A photo sensor can be used to detect whether it is day or night time to adjust modes. For instance, a night mode would turn the LEDS on a dim mode when not braking to increase nighttime visibility like a car has rear tail lights that are constantly on at night. If additional time, we may add features like distance checking via ultrasonic sensors or a display for the user. The ultrasonic sensor can detect objects coming at high velocities by measuring the change of distance over time. If something is detected to be approaching quickly, a buzzer can be activated to alert the user.

### 9. Components

What major components do you need and why?

We will need a low-power usage and small form factor microcontroller to run most of our code, as well as the following sensors:

- ultrasonic sensor (waterproof / enclosed)
- photosensor (for daylight)
- real time clock chip (for current time/date + duration of trip)
- Bright LED or light for brake light
- LED/light to attract attention of user when car is near
- accelerometer (for deceleration data for breaking)
- display (for displaying accelerometer data + )
- power management/voltage supervisor circuit

### 10. Evaluation

What is your metric for evaluating how well your product/solution solves the problem? Think critically on this section. Having a boolean metric such as “it works” is not very useful. This is akin to making a speaker and if it emits sound, albeit however terrible and ear wrenching, declare this a success.
It is recommended that your project be something that you can take pride in. Oftentimes in interviews, you will be asked to talk about projects you have worked on.

The metric by which we will evaluate the success of our product/solution will be whether it meaningfully results in a change in behavior for the rider and drivers. We can test this measuring the average following distance of cars at night with the device on vs. off, and comparing the time the rider takes to notice when a car approaches them using the ultrasonic sensor, with it on and off.

### 11. Timeline

This section is to help guide your progress over the next few weeks. Feel free to adjust and edit the table below to something that would be useful to you. Really think about what you want to accomplish by the first milestone.

| **Week**            | **Task** | **Assigned To**    |
|----------           |--------- |------------------- |
| Week 1: 3/24 - 3/31 |   look for parts & basic flow charts   |     both     |
| Week 2: 4/1 - 4/7   |   begin 3d render software -- Basic software & serial communication  |     Maya -- Chris    |
| Week 3: 4/8 - 4/14  |   Continue working on software -- help software, begin hardware   |    both     |
| Week 4: 4/15 - 4/21 |   Finalize hardware -- CAD   |      Maya -- Chris        |
| Week 5: 4/22 - 4/26 |   Test & Debug    |     both    |

### 12. Proposal Presentation

Add your slides to the Final Project Proposal slide deck in the Google Drive.

## Final Project Report

Don't forget to make the GitHub pages public website!
If you’ve never made a Github pages website before, you can follow this webpage (though, substitute your final project repository for the Github username one in the quickstart guide):  <https://docs.github.com/en/pages/quickstart>

### 1. Video

[Insert final project video here]

### 2. Images

[Insert final project images here]

### 3. Results

What were your results? Namely, what was the final solution/design to your problem?

#### 3.1 Software Requirements Specification (SRS) Results

Based on your quantified system performance, comment on how you achieved or fell short of your expected software requirements. You should be quantifying this, using measurement tools to collect data.

#### 3.2 Hardware Requirements Specification (HRS) Results

Based on your quantified system performance, comment on how you achieved or fell short of your expected hardware requirements. You should be quantifying this, using measurement tools to collect data.

### 4. Conclusion

Reflect on your project. Some questions to consider: What did you learn from it? What went well? What accomplishments are you proud of? What did you learn/gain from this experience? Did you have to change your approach? What could have been done differently? Did you encounter obstacles that you didn’t anticipate? What could be a next step for this project?

## References

Fill in your references here as you work on your proposal and final submission. Describe any libraries used here.

## Github Repo Submission Resources

You can remove this section if you don't need these references.

* [ESE5160 Example Repo Submission](https://github.com/ese5160/example-repository-submission)
* [Markdown Guide: Basic Syntax](https://www.markdownguide.org/basic-syntax/)
* [Adobe free video to gif converter](https://www.adobe.com/express/feature/video/convert/video-to-gif)
* [Curated list of example READMEs](https://github.com/matiassingers/awesome-readme)
* [VS Code](https://code.visualstudio.com/) is heavily recommended to develop code and handle Git commits
  * Code formatting and extension recommendation files come with this repository.
  * Ctrl+Shift+V will render the README.md (maybe not the images though)
