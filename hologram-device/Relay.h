#ifndef RELAY_H
#define RELAY_H

class Relay {
    public:
        enum Mode {
            NORMALLY_OPEN,
            NORMALLY_CLOSED
        };

        Relay(int, Mode);

        int GetPin();
        Mode GetMode();

        void Activate();
        void Deactivate();
        void Open();
        void Close();

    private:
        int m_pin;
        Mode m_mode;

};

#endif

