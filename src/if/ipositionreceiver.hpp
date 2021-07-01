#ifndef IPOSITIONRECEIVER_HPP
#define IPOSITIONRECEIVER_HPP

#include <utility>
#include "position.hpp"

/**
 * @brief The IPositionReceiver interface
 *
 * Implementers of this interface
 * will be updated about robot position
 */
class IPositionReceiver
{
public:
    enum PositionState{
        POSITION_STATE_FALL,
        POSITION_STATE_NOT_ATTACHED,
        POSITION_STATE_ATTACHED
    };

    virtual ~IPositionReceiver() = default;

    virtual void updatePosition(const position_t &pos) = 0;
    virtual void updateConnectionState(PositionState state) = 0;
};

/**
 * @brief The IPositionNotifier interface
 *
 * Implementers of this interface
 * must notify about robot position using IPositionReceiver
 */
class IPositionNotifier
{
public:
    virtual ~IPositionNotifier() = default;

    virtual void setPositionReceiver(IPositionReceiver *receiver) = 0;
};


#endif // IPOSITIONRECEIVER_HPP
