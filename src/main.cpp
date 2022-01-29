#include "main.h"
#define clampPiston 'B'
#define anglerPiston 'A'
std::shared_ptr<ChassisController> drive =
	ChassisControllerBuilder()
		.withMotors( {-11,-12},{1,2})
		// Green gearset, 4 in wheel diam, 11.5 im wheel track
		// 36 to 60 gear ratio
		.withDimensions({AbstractMotor::gearset::blue, (60.0/36.0)},{{3.25_in, 11_in}, imev5GreenTPR})
		.build();
		Controller controller;
		pros::ADIDigitalOut clamp (clampPiston);
		pros::ADIDigitalOut angler (anglerPiston);
		ControllerButton clampButton (ControllerDigital::R1);
		ControllerButton anglerButton (ControllerDigital::R2);
		ControllerButton liftUpButton (ControllerDigital::L1);
		ControllerButton liftDownButton (ControllerDigital::L2);
		bool isClampClosed = false;
		bool isAnglerLifted =false;
		MotorGroup lift {-3,13};
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
	clamp.set_value(true);
	/*
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
	*/
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
void autonomous() {}

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
	while (true) {
		drive->getModel() -> arcade(controller.getAnalog(ControllerAnalog::leftY), controller.getAnalog(ControllerAnalog::rightX));
		pros::delay(10);
		if (clampButton.isPressed())
		{
			if(isClampClosed){
			clamp.set_value(false);
			isClampClosed=false;
			pros::delay(200);
		}else{
			clamp.set_value(true);
			isClampClosed=true;
			pros::delay(200);
		}
		}

		if (anglerButton.isPressed())
		{
			if(isAnglerLifted){
			angler.set_value(false);
			isAnglerLifted=false;
			pros::delay(200);
		}else{
			angler.set_value(true);
			isAnglerLifted=true;
			pros::delay(200);
		}
		}

		if (liftUpButton.isPressed())
		{
			lift.moveVelocity(-100);
		if (liftDownButton.isPressed())
		{
			lift.moveVelocity(100);
		}
		//pros::delay(20);
	}
}
}
