#include "Relay.h"
#include "Arduino.h"

Relay::Relay(int p, Relay::Mode m) : m_pin(p), m_mode(m) {
    pinMode(m_pin, OUTPUT);
    Deactivate();    
}

int Relay::GetPin() {
    return m_pin;
}

Relay::Mode Relay::GetMode() {
    return m_mode;
}

void Relay::Activate() {
    digitalWrite(m_pin, LOW);
}

void Relay::Deactivate() {
    digitalWrite(m_pin, HIGH);
}

void Relay::Open() {
    (m_mode == NORMALLY_OPEN) ? Deactivate() : Activate(); 
}

void Relay::Close() {
    (m_mode == NORMALLY_OPEN) ? Activate() : Deactivate(); 
}

