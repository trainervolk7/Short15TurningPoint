#include "main.h"
#define lowerPiston 'A'
#define liftPiston 'B'

std::shared_ptr<ChassisController> drive =
	ChassisControllerBuilder()
		.withMotors( {-18, 19, -20},{11, -12, 13})
		// Green gearset, 4 in wheel diam, 11.5 im wheel track
		// 36 to 60 gear ratio
		.withDimensions({AbstractMotor::gearset::blue},{{2.75_in, 12_in}, imev5BlueTPR}) //imev5GreenTPR
		.build();
		std::shared_ptr<AsyncMotionProfileController> profileController =
				  AsyncMotionProfileControllerBuilder()
				    .withLimits({
				      15.0, // Maximum linear velocity of the Chassis in m/s
				      10.0, // Maximum linear acceleration of the Chassis in m/s/s
				      10.0 // Maximum linear jerk of the Chassis in m/s/s/s
				    })
				    .withOutput(drive)
						.buildMotionProfileController();
		Controller controller;
		pros::ADIDigitalOut lowerClamp (lowerPiston);
		pros::ADIDigitalOut LiftClamp (liftPiston);
		ControllerButton lowerClampButton (ControllerDigital::R2);
		ControllerButton LiftClampButton (ControllerDigital::R1);
		ControllerButton liftUpButton (ControllerDigital::L2);   // fix naming or button (Actually moves down)
		ControllerButton liftDownButton (ControllerDigital::L1); // (Actually moves up)
		ControllerButton ringIntakeButton (ControllerDigital::Y);
		ControllerButton ringNonIntakeButton (ControllerDigital::B);
		ControllerButton liftGoalPosition (ControllerDigital::X);

		ControllerButton setGoalPosition (ControllerDigital::up);
		bool isInGoalPosition = false;
		bool isInRingPosition = false;

		bool isLowerClampClosed = false;
		bool isLiftClampClosed =false;
		bool isRingOn = false;
		//Motor lift {14};
		Motor ringMotor {-17};
		std::shared_ptr<AsyncPositionController<double, double>> liftControl =
	AsyncPosControllerBuilder()
		.withMotor(lift)
		.build();

		std::shared_ptr<AsyncPositionController<double, double>> ringChainControl  =
	AsyncPosControllerBuilder()
		.withMotor(ringMotor)
		.build();



/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	/*
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
	*/
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	lowerClamp.set_value(true);
	LiftClamp.set_value(true);
	lift.setBrakeMode(okapi::AbstractMotor::brakeMode(2));
	lift.setGearing(okapi::AbstractMotor::gearset::red);
	ringMotor.setGearing(okapi::AbstractMotor::gearset::blue);
	ringMotor.tarePosition();
	lift.setEncoderUnits(okapi::AbstractMotor::encoderUnits::rotations);
	lift.tarePosition();

	profileController->generatePath(
	{{0_in, 0_in, 0_deg}, {58_in, -12_in, 25_deg}}, "A"); ///A works
	profileController->generatePath(
	{{0_in, 0_in, 0_deg}, {20_in, 25_in, 25_deg}}, "B");
	profileController->generatePath(
	{{0_in, 0_in, 0_deg}, {22_in, -9_in, -35_deg}}, "C"); //21 -4
	profileController->generatePath(
	{{0_in, 0_in, 0_deg}, {30_in, -9_in, -35_deg}}, "D");
	profileController->generatePath(
	{{0_in, 0_in, 0_deg}, {47_in, 0_in, 0_deg}}, "E");
	profileController->generatePath(
	{{0_in, 0_in, 0_deg}, {20_in, 0_in, 0_deg}}, "Straight6");
	profileController->generatePath(
	{{0_in, 0_in, 0_deg}, {24_in, 0_in, 0_deg}}, "Straight24");
	profileController->generatePath(
	{{0_in, 0_in, 0_deg}, {20_in, 0_in, 0_deg}}, "prepare");
	profileController->generatePath(
	{{0_in, 0_in, 0_deg}, {6_in, 0_in, 0_deg}}, "RingLoad");

	screen::init();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	lift.setBrakeMode(AbstractMotor::brakeMode(2));
	//liftControl->setTarget(0.16);

	liftControl->setTarget(0.16);
	LiftClamp.set_value(false);
	//Drive to Yellow Goal
	profileController->setTarget("A");
	profileController->waitUntilSettled();
	LiftClamp.set_value(true);
	lowerClamp.set_value(false);
	//Drive to Blue Goal
	profileController->setTarget("B",true);
	profileController->waitUntilSettled();
	profileController->setTarget("C",true);
	profileController->waitUntilSettled();
	lowerClamp.set_value(true);
	//liftControl->setTarget(2.026);
	//Drive to Platform for yellow
	profileController->setTarget("D");
	profileController->waitUntilSettled();
	liftControl->setTarget(2.026);
	pros::delay(1000);
	drive ->turnAngle(180_deg);
	/*pros::delay(250);
	profileController->setTarget("E");
	profileController->waitUntilSettled();
	liftControl->setTarget(2.026);
	pros::delay(1000);
	drive ->turnAngle(90_deg);
	pros::delay(500);
	profileController->setTarget("Straight24");
	profileController->waitUntilSettled();
	LiftClamp.set_value(false);
	profileController->setTarget("Straight24",true);
	profileController->waitUntilSettled();
	drive ->turnAngle(90_deg);
	pros::delay(250);
	ringMotor.moveVelocity(233);
	profileController->setTarget("E");
	profileController->waitUntilSettled();
	drive ->turnAngle(-90_deg);
	pros::delay(250);*/
	ringMotor.moveVelocity(233);
	profileController->setTarget("prepare");
	profileController->waitUntilSettled();
	//Ring Loading
	for(int i = 1; i<=13; i++)
	{
	profileController->setTarget("RingLoad");
	profileController->waitUntilSettled();
	profileController->setTarget("RingLoad",true);
	profileController->waitUntilSettled();
	}
	//drive->turnAngle(30_deg);
	/*profileController->setTarget("B");
	profileController->waitUntilSettled();
	drive->turnAngle(-30_deg);
	profileController->setTarget("C");
	profileController->waitUntilSettled();
	*/









	//while (true){
	// std::shared_ptr<AsyncMotionProfileController> profileController =
	// 		  AsyncMotionProfileControllerBuilder()
	// 		    .withLimits({
	// 		      1.0, // Maximum linear velocity of the Chassis in m/s
	// 		      2.0, // Maximum linear acceleration of the Chassis in m/s/s
	// 		      changeVelocity // Maximum linear jerk of the Chassis in m/s/s/s
	// 		    })
	// 		    .withOutput(drive)
	// 		    .buildMotionProfileController();
	// 			}
		// profileController->generatePath(
    // {{0_ft, 0_ft, 0_deg}, {3_ft, 0_ft, 0_deg}}, "A");
		// changeVelocity=1.0;
		// profileController->generatePath(
		// {{0_ft, 0_ft, 0_deg}, {3_ft, 0_ft, 0_deg}}, "B");
		/*lowerClamp.set_value(false);
		lift.setBrakeMode(AbstractMotor::brakeMode(2));
		liftControl->setTarget(-100);
		profileController->generatePath(
		{{0_in, 0_in, 0_deg}, {29_in, 0_in, 0_deg}}, "A");
		profileController->generatePath(
		{{0_in, 0_in, 0_deg}, {65_in, 0_in, 0_deg}}, "B");
		profileController->generatePath(
		{{0_in, 0_in, 0_deg}, {13_in, 0_in, 0_deg}}, "C");
		profileController->generatePath(
		{{0_in, 0_in, 0_deg}, {15_in, 0_in, 0_deg}}, "D");

		profileController->generatePath(
		{{0_in, 0_in, 0_deg}, {25_in, 0_in, 0_deg}}, "E");
		profileController->generatePath(
		{{0_in, 0_in, 0_deg}, {3_in, 0_in, 0_deg}}, "F");

		profileController->setTarget("A");
		profileController->waitUntilSettled();
		lowerClamp.set_value(true);
		pros::delay(20);
		ringMotor.moveVelocity(300);
		pros::delay(2500);
		ringMotor.moveVelocity(0);
		liftControl->setTarget(-700);
		pros::delay(1000);
		profileController->setTarget("E",true);
		profileController->waitUntilSettled();
		lowerClamp.set_value(false);
		profileController->setTarget("F",true);
		profileController->waitUntilSettled();
		*/
		/*drive->turnAngle(-32_deg);
		profileController->setTarget("B");
		profileController->waitUntilSettled();
		pros::delay(1000);
		liftControl->setTarget(-2100);
		pros::delay(1000);
		drive->turnAngle(-32_deg);
		profileController->setTarget("C");
		profileController->waitUntilSettled();
		pros::delay(1000);
		liftControl->setTarget(-1500);
		lowerClamp.set_value(false);
		liftControl->setTarget(-2100);
		profileController->setTarget("D",true);
*/
		//drive->turnAngle(-32_deg);


	}
/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	lowerClamp.set_value(true);
	isLowerClampClosed=true;
	LiftClamp.set_value(true);
	isLiftClampClosed=true;

	while (true) {
		drive->getModel() -> arcade(controller.getAnalog(ControllerAnalog::leftY), controller.getAnalog(ControllerAnalog::rightX));
		pros::delay(10);
		if (lowerClampButton.isPressed())
		{
			if(isLowerClampClosed){
			lowerClamp.set_value(false);
			isLowerClampClosed=false;
			pros::delay(200);
		}else{
			lowerClamp.set_value(true);
			isLowerClampClosed=true;
			pros::delay(200);
		}
		}

		if (LiftClampButton.isPressed())
		{
			if(isLiftClampClosed){
			LiftClamp.set_value(false);
			isLiftClampClosed=false;
			pros::delay(200);
		}else{
			LiftClamp.set_value(true);
			isLiftClampClosed=true;
			pros::delay(200);
		}
		}

		if (liftUpButton.changedToPressed())
		{
			lift.moveVelocity(-200);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(-500);
			}
		}
		else if(liftUpButton.changedToReleased())
		{
			lift.moveVoltage(0);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(-500);
			}
		}
		else if(liftDownButton.changedToPressed())
		{
			lift.moveVelocity(200);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(-500);
			}
		}
		else if (liftDownButton.changedToReleased())
		{
			lift.moveVoltage(0);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(-500);
			}
		}

		if (ringIntakeButton.isPressed())
	{
		if (!(lift.getPosition() >= 0.45))
		{
				liftControl->setTarget(0.46);
				//pros::delay(100);
		}
		if (isRingOn == false) {
			ringMotor.moveVelocity(233);
			isRingOn = true;
		} else {
			ringMotor.moveVelocity(0);
			isRingOn = false;
		}
		pros::delay(200);
	}

	if (ringNonIntakeButton.isPressed())
	{
		if (!(lift.getPosition() >= 0.45))
		{
				liftControl->setTarget(0.46);
		}
		if (isRingOn == false) {
			ringMotor.moveVelocity(-233);
			isRingOn = true;
		} else {
			ringMotor.moveVelocity(0);
			isRingOn = false;
		}
		pros::delay(200);
	}

	if (liftGoalPosition.isPressed())
	{
		ringMotor.moveVelocity(0);
		isRingOn = false;
		//ringMotor.moveAbsolute(0, 600);
		//pros::delay(1000);
		if (!(lift.getPosition() >= 0.15 && lift.getPosition() <= 0.17))
		{
				liftControl->setTarget(0.16);
		}
	}


		//pros::delay(20);
	}

}
