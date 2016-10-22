namespace hardware
{
    class motor
    {
        /**
         * @brief      Sets the acceleration.
         *
         * @param[in]  acceleration  The acceleration in m/s^2.
         */
        virtual void set_acceleration(double acceleration) = 0;

        /**
         * @brief      Increment or decrement the acceleration.
         *
         * @param[in]  delta  The change in acceleration in m/s^2.
         */
        virtual void change_acceleration(double delta) = 0;

        /**
         * @brief      Get the position of the motor, determined by integrating velocity.
         *
         * @return     The distance in m travelled by a point on the edge of the wheel, plus an
         *             offset determined by the last call to calibrate.
         */
        virtual double position() const = 0;

        /**
         * @brief      Get the velocity of the motor in m/s.
         */
        virtual double velocity() const = 0;

        /**
         * @brief      Get the acceleration of the motor in m/s^2.
         */
        virtual double acceleration() const = 0;

        /**
         * @brief      Set the position of the motor.
         *
         * @param[in]  new_position  The new position in m.
         */
        virtual void calibrate(float new_position) = 0;

        virtual ~motor() {}
    };
}
