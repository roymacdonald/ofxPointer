//
// Copyright (c) 2009 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier: MIT
//


#pragma once


#include <map>
#include <set>
#include <string>
#include "json.hpp"
#include "ofEvents.h"
#include "ofColor.h"
#include "ofUtils.h"
#include "ofAppBaseWindow.h"
#include "ofAppRunner.h"
#include "ofRectangle.h"
#include "ofLog.h"


namespace ofx {


class PointShape;
class Point;
class PointerEventArgs;


/// \brief A base class describing the basic components of event arguments.
///
/// Loosely based on DOM events.
///
/// \sa https://dom.spec.whatwg.org/
class EventArgs: public ofEventArgs
{
public:
    /// \brief Create a default EventArgs.
    EventArgs();

    /// \brief Create a BaseEventArgs with the given paramaters.
    /// \param eventSource The source of the event.
    /// \param eventType A string describing the type of the event.
    /// \param timestampMicros The timestamp of the event in microseconds.
    /// \param detail Optional event detail.
    EventArgs(const void* eventSource,
              const std::string& eventType,
              uint64_t timestampMicros,
              uint64_t detail);

    /// \brief Destroy the EventArgs.
    virtual ~EventArgs();

    /// \returns the source of the event if available.
    const void* eventSource() const;

    /// \returns the event type.
    std::string eventType() const;

    /// \returns the timestamp of this event in milliseconds.
    uint64_t timestampMillis() const;

    /// \returns the timestamp of this event in microseconds.
    uint64_t timestampMicros() const;

    /// \returns the optional event detail.
    uint64_t detail() const;

    /// \brief An unknown event type.
    static const std::string EVENT_TYPE_UNKNOWN;

private:
    /// \brief A pointer to the event source.
    const void* _eventSource = nullptr;

    /// \brief The name of this event type.
    /// \sa https://dom.spec.whatwg.org/#dom-event-type
    std::string _eventType;

    /// \brief The timestamp of this event in microseconds.
    uint64_t _timestampMicros = 0;

    /// \brief Detail for the event.
    ///
    /// Specifies some detail information about the event, depending on the type
    /// of event.
    ///
    /// \sa https://dom.spec.whatwg.org/#dom-customevent-detail
    uint64_t _detail = 0;

};


/// \brief A PointShape describes the shape of a pointer.
///
/// For standard pointers, such as a mouse or a pen, the width and height of the
/// PointShape will be set to 1 while some touch pointers may define the
/// size of the touch or even an ellipse describing the size and angle of a
/// finger tip. Pens or other input may define a width and height smaller than
/// 1.
class PointShape
{
public:
    /// \brief The type of the point shape.
    enum class ShapeType
    {
        ELLIPSE, /// \brief Interpret width, height and angle as a rotated ellipse.
        RECTANGLE /// \brief Interpret width, height and angle as a rotated rectangle.
    };

    /// \brief Create a default PointShape.
    PointShape();

    /// \brief Create a square or circular PointShape with parameters.
    /// \param shapeType The type of the shape.
    /// \param size The diameter or width / height of the shape.
    /// \param sizeTolerance The tolerance of the diameter or width / height.
    PointShape(ShapeType shapeType,
               float size,
               float sizeTolerance);

    /// \brief Create a PointShape with parameters.
    /// \param shapeType The type of the shape.
    /// \param width The width of the shape.
    /// \param height The height of the shape.
    /// \param widthTolerance The width tolerance of the shape.
    /// \param heightTolerance The height tolerance of the shape.
    /// \param angleDeg The shape angle in degrees.
    PointShape(ShapeType shapeType,
               float width,
               float height,
               float widthTolerance,
               float heightTolerance,
               float angleDeg);

    /// \brief Destroy the PointShape.
    virtual ~PointShape();

    /// \returns the ShapeType, which determines how to intepret dimensions.
    ShapeType shapeType() const;

    /// \returns the width the shape.
    float width() const;

    /// \returns the height of the shape.
    float height() const;

    /// \brief Get the tolerance of the shape width.
    ///
    /// The width range of the shape will be:
    ///     width() + / - widthTolerance()
    ///
    /// \returns the tolerance of the shape width.
    float widthTolerance() const;

    /// \brief Get the tolerance of the shape height.
    ///
    /// The width range of the shape will be:
    ///     height() + / - heightTolerance()
    ///
    /// \returns the tolerance of the shape height.
    float heightTolerance() const;

    /// \returns the the angle of the shape in degrees.
    float angleDeg() const;

    /// \returns the the angle of the shape in radians.
    float angleRad() const;

    /// \brief Get the axis-aligned width of the shape.
    ///
    /// This will be equal to width if the shape or rotation is 0.
    ///
    /// \returns the axis-aligned width of the shape.
    float axisAlignedWidth() const;

    /// \brief Get the axis-aligned height of the shape.
    ///
    /// This will be equal to height if the shape or rotation is 0.
    ///
    /// \returns the axis-aligned width of the shape.
    float axisAlignedHeight() const;

protected:
    /// \brief Shape type for the pointer.
    ShapeType _shapeType = ShapeType::ELLIPSE;

    /// \brief The width of the rotated shape.
    float _width = 1;

    /// \brief The height of the rotated shape.
    float _height = 1;

    /// \brief Width tolerance.
    float _widthTolerance = 0;

    /// \brief Height tolerance.
    float _heightTolerance = 0;

    /// \brief Shape angle in degrees.
    float _angleDeg = 0;

    /// \brief Caclulate AABB for the shape on demand.
    void _calculateAxisAlignedSize() const;

    /// \brief True if the axis alignment was cached.
    mutable bool _axisAlignedSizeCached = false;

    /// \brief Axis-aligned bounding box width.
    mutable float _axisAlignedWidth = 0;

    /// \brief Axis-aligned box height.
    ///
    /// _height == 0 means the _height has not yet been calculated.
    mutable float _axisAlignedHeight = 0;

    friend Point;
    friend PointerEventArgs;

};


inline std::string to_string(PointShape::ShapeType v)
{
    switch (v)
    {
        case PointShape::ShapeType::ELLIPSE:
            return "ELLIPSE";
        case PointShape::ShapeType::RECTANGLE:
            return "RECTANGLE";
    }

    return "ELLIPSE";
}


inline void to_json(nlohmann::json& j, const PointShape::ShapeType& v)
{
    j = to_string(v);
}


inline void from_json(const nlohmann::json& j, PointShape::ShapeType& v)
{
    std::string s = j.get<std::string>();
    if (!s.empty())
    {
        if (s == to_string(PointShape::ShapeType::ELLIPSE))
        {
            v = PointShape::ShapeType::ELLIPSE;
            return;
        }
        else if (s == to_string(PointShape::ShapeType::RECTANGLE))
        {
            v = PointShape::ShapeType::RECTANGLE;
            return;
        }
    }

    ofLogWarning("from_json") << "Unknown value: " << s;
    v = PointShape::ShapeType::ELLIPSE;
    return;
}


inline void to_json(nlohmann::json& j, const PointShape& v)
{
    j = {
        { "shape_type", v.shapeType() },
        { "width", v.width() },
        { "height", v.height() },
        { "width_tolerance", v.widthTolerance() },
        { "height_tolerance", v.heightTolerance() },
        { "angle_deg", v.angleDeg() }
    };
}


inline void from_json(const nlohmann::json& j, PointShape& v)
{
    v = PointShape(j.value("shape_type", PointShape::ShapeType::ELLIPSE),
                   j.value("width", float(1)),
                   j.value("height", float(1)),
                   j.value("width_tolerance", float(0)),
                   j.value("height_tolerance", float(0)),
                   j.value("angle_deg", float(0)));
}


/// \brief A class representing a pointer's point.
///
/// Captures all position, shape, tilt rotation and pressure information.
class Point
{
public:
    /// \brief Construct a Point
    Point();

    /// \brief Construct a Point
    /// \param position The position in screen coordinates.
    Point(const glm::vec2& position);

    /// \brief Construct a Point
    /// \param position The position in screen coordinates.
    /// \param shape The point shape.
    Point(const glm::vec2& position, const PointShape& shape);

    /// \brief Construct a Point
    /// \param position The position in screen coordinates.
    /// \param pressure The normalized pressure.
    /// \param tiltXDeg The tilt X angle in degrees.
    /// \param tiltYDeg The tilt Y angle in degrees.
    Point(const glm::vec2& position, float pressure, float tiltXDeg, float tiltYDeg);

    /// \brief Construct a Point
    /// \param position The position in screen coordinates.
    /// \param shape The point shape.
    /// \param pressure The normalized pressure.
    Point(const glm::vec2& position, const PointShape& shape, float pressure);

    /// \brief Construct a Point
    /// \param position The position in screen coordinates.
    /// \param precisePosition The precise position in screen coordinates.
    /// \param shape The point shape.
    /// \param pressure The normalized pressure.
    /// \param tangentialPressure The tangential pressure (aka barrel pressure).
    /// \param twistDeg The twist angle in degrees.
    /// \param tiltXDeg The tilt X angle in degrees.
    /// \param tiltYDeg The tilt Y angle in degrees.
    Point(const glm::vec2& position,
          const glm::vec2& precisePosition,
          const PointShape& shape,
          float pressure,
          float tangentialPressure,
          float twistDeg,
          float tiltXDeg,
          float tiltYDeg);

    /// \brief Destroy the Point.
    virtual ~Point();

    /// \returns the position in screen coordinates.
    glm::vec2 position() const;

    /// \returns the precise position in screen coordinates.
    glm::vec2 precisePosition() const;

    /// \brief Get the normalized point pressure.
    ///
    /// The normalized pressure is in the range [0, 1].  For devices that do not
    /// support pressure, the value is 0.5 when a button is active or 0
    /// otherwise.
    ///
    /// \returns the normalized point pressure [0, 1].
    float pressure() const;

    /// \brief Get the Point's normalized tangential pressure.
    ///
    /// The normalized tangential pressure (aka the barrel pressure) is in the
    /// range [0, 1].  For devices that do not support tangential pressure, the
    /// value is 0.
    ///
    /// \returns the normalized tangential pressure [0, 1].
    float tangentialPressure() const;

    /// \brief Get the Point's twist in degrees.
    ///
    /// The clockwise rotation (in degrees, in the range of [0,359]) of a
    /// transducer (e.g. pen stylus) around its own major axis. For hardware and
    /// platforms that do not report twist, the value MUST be 0.
    ///
    /// \returns the twist in degrees.
    float twistDeg() const;

    /// \brief Get the Point's twist in radians.
    ///
    /// The clockwise rotation (in degrees, in the range of [0,2*PI]) of a
    /// transducer (e.g. pen stylus) around its own major axis. For hardware and
    /// platforms that do not report twist, the value MUST be 0.
    ///
    /// \returns the twist in radians.
    float twistRad() const;

    /// \brief Get the Tilt X angle in degrees.
    ///
    /// Tilt X is given in degrees [-90, 90] between the Y-Z plane and the plane
    /// containing both the transducer (e.g. pen stylus) axis and the Y axis.  A
    /// positive tilt X is to the right.  The value is 0 if the tilt X is
    /// undefined.
    ///
    /// \returns the Tilt X angle in degrees.
    float tiltXDeg() const;

    /// \brief Get the Tilt X angle in radians.
    ///
    /// Tilt X is given in degrees [-PI/2, PI/2] between the Y-Z plane and the plane
    /// containing both the transducer (e.g. pen stylus) axis and the Y axis.  A
    /// positive tilt X is to the right.  The value is 0 if the tilt X is
    /// undefined.
    ///
    /// \returns the Tilt X angle in radians.
    float tiltXRad() const;

    /// \brief Get the Tilt Y angle in degrees.
    ///
    /// Tilt Y is given in degrees [-90, 90] between the X-Z plane and the plane
    /// containing both the transducer (e.g. pen stylus) axis and the X axis.  A
    /// positive tilt Y is toward the user.  The value is 0 if the tilt Y is
    /// undefined.
    ///
    /// \returns the Tilt Y angle in degrees.
    float tiltYDeg() const;

    /// \brief Get the Tilt Y angle in radians.
    ///
    /// Tilt Y is given in degrees [-PI/2, PI/2] between the X-Z plane and the plane
    /// containing both the transducer (e.g. pen stylus) axis and the X axis.  A
    /// positive tilt Y is toward the user.  The value is 0 if the tilt Y is
    /// undefined.
    ///
    /// \returns the Tilt Y angle in radians.
    float tiltYRad() const;

    /// \brief Get the azimuth angle in degress.
    ///
    /// The azimuth angle, in the range 0 to 360 degrees. 0 represents a stylus
    /// whose cap is pointing in the direction of increasing screen X values.
    /// 180 degrees represents a stylus whose cap is pointing in the direction
    /// of increasing screen Y values. The value is 0 if undefined.
    ///
    /// \returns the azimuth in degress.
    float azimuthDeg() const;

    /// \brief Get the azimuth angle in radians.
    ///
    /// The azimuth angle, in the range 0 to 2π radians. 0 represents a stylus
    /// whose cap is pointing in the direction of increasing screen X values.
    /// π/2 radians represents a stylus whose cap is pointing in the direction
    /// of increasing screen Y values. The value is 0 if undefined.
    ///
    /// \returns the azimuth in degress.
    float azimuthRad() const;

    /// \brief Get the altitude angle in degrees.
    ///
    /// The altitude angle, in the range 0 degrees (parallel to the surface) to
    /// 90 degrees (perpendicular to the surface). The value is 0 if undefined.
    ///
    /// \returns the altitude in degrees.
    float altitudeDeg() const;

    /// \brief Get the altitude angle in degrees.
    ///
    /// The altitude angle, in the range 0 radians (parallel to the surface) to
    /// π/2 radians (perpendicular to the surface). The value is 0 if undefined.
    ///
    /// \returns the altitude in radians.
    float altitudeRad() const;

    /// \brief Get the shape of the Point.
    /// \returns The PointShape.
    const PointShape& shape() const;

    /// \brief A debug utility for viewing the contents of Point.
    /// \returns A string representation of the Point.
    std::string toString() const
    {
        std::stringstream ss;

        ss << "------------" << std::endl;
        ss << "        Position: " << position().x << "," << position().y << std::endl;
        ss << "Precise Position: " << precisePosition().x << "," << precisePosition().y << std::endl;
        ss << "        Pressure: " << pressure() << std::endl;
        ss << "   Tan. Pressure: " << tangentialPressure() << std::endl;
        ss << "           Twist: " << twistDeg() << std::endl;
        ss << "           TiltX: " << tiltXDeg() << std::endl;
        ss << "           TiltY: " << tiltYDeg() << std::endl;
//        ss << "           Shape: " << std::endl << shape().toString();

        return ss.str();
    }


private:
    /// \brief The position in screen coordinates.
    glm::vec2 _position;

    /// \brief The Point's precise position if available.
    ///
    /// If a high resolution position is available, it will be set. Otherwise,
    /// this position will be equal to the _position variable.
    ///
    /// Typically, the _precisePosition should not be used for hit-testing, but
    /// rather the _position should.
    ///
    /// For example, this is available from Apple's UITouch.
    glm::vec2 _precisePosition;

    /// \brief The Point shape.
    PointShape _shape;

    /// \brief The Point's normalized pressure.
    float _pressure = 0;

    /// \brief The Point's tangential pressure (aka barrel pressure).
    float _tangentialPressure = 0;

    /// \brief The Point's twist in degrees.
    float _twistDeg = 0;

    /// \brief The Point tilt X angle in degrees.
    float _tiltXDeg = 0;

    /// \brief The Point tilt Y angle in degrees.
    float _tiltYDeg = 0;

    /// \brief A utility function for caching the azimuth and altitude.
    void _cacheAzimuthAltitude() const;

    /// \brief True if the azimuth and altitude are cached.
    mutable bool _azimuthAltitudeCached = false;

    /// \brief The cached azimuth in degrees.
    mutable float _azimuthDeg = 0;

    /// \brief The cached altitude in degrees.
    mutable float _altitudeDeg = 0;

    friend PointerEventArgs;
};


// \todo This function is a replacement for automatic glm serialization using ofxSerializer.
inline glm::vec2 to_vec_2_temp(const nlohmann::json& j)
{
    return glm::vec2(j.value("x", float(0)),
                     j.value("y", float(0)));
}


// \todo This function is a replacement for automatic glm serialization using ofxSerializer.
inline nlohmann::json to_json_temp(const glm::vec2& v)
{
    nlohmann::json j = {
        { "x", v.x },
        { "y", v.y }
    };
    return j;
}


inline void to_json(nlohmann::json& j, const Point& v)
{
    j =
    {
        { "position", to_json_temp(v.position()) },
        { "precise_position", to_json_temp(v.precisePosition()) },
        { "shape", v.shape() },
        { "pressure", v.pressure() },
        { "tangential_pressure", v.tangentialPressure() },
        { "twist_deg", v.twistDeg() },
        { "tilt_x_deg", v.tiltXDeg() },
        { "tilt_y_deg", v.tiltYDeg() }
    };
}


inline void from_json(const nlohmann::json& j, Point& v)
{
    nlohmann::json jp = j.value("position", nlohmann::json());
    nlohmann::json jpp = j.value("precise_position", nlohmann::json());

    v = Point(to_vec_2_temp(jp),
              to_vec_2_temp(jpp),
              j.value("shape", PointShape()),
              j.value("pressure", float(0)),
              j.value("tangential_pressure", float(0)),
              j.value("twist_deg", float(0)),
              j.value("tilt_x_deg", float(0)),
              j.value("tilt_y_deg", float(0)));
}


/// \brief A class representing all of the arguments in a pointer event.
///
/// PointerEventArgs are usually passed as arguments in the openFrameworks event
/// system, for example, POINTER_UP events might be dispatched to an event like:
///
///     ofEvent<PointerEventArgs> onPointerUp;
///
/// \sa https://w3c.github.io/pointerevents/
/// \sa https://w3c.github.io/pointerevents/extension.html
class PointerEventArgs: public EventArgs
{
public:
    /// \brief Create a default PointerEventArgs.
    PointerEventArgs();

    /// \brief Create a copy of the event with a new event type.
    /// \param eventType The new event type.
    /// \param event the event to copy.
    PointerEventArgs(const std::string& eventType,
                     const PointerEventArgs& event);

    /// \brief Create a PointerEventArgs with parameters.
    /// \param eventSource The event source if available.
    /// \param eventType The pointer event type.
    /// \param timestampMicros The timestamp of this event in microseconds
    /// \param detail The optional event details.
    /// \param point The point.
    /// \param pointerId The unique pointer id.
    /// \param deviceId The unique input device id.
    /// \param pointerIndex The unique pointer index for the given device id.
    /// \param sequenceIndex The sequence index for this event or zero if not supported..
    /// \param deviceType The device type string.
    /// \param isCoalesced Is this event delivered as coalesced.
    /// \param isPredicted Is this event predicted rather than measured.
    /// \param isPrimary True if this pointer is the primary pointer.
    /// \param button The button id for this event.
    /// \param buttons All pressed buttons for this pointer.
    /// \param modifiers All modifiers for this pointer.
    /// \param buttons All pressed buttons for this pointer.
    /// \param modifiers All modifiers for this pointer.
    /// \param coalescedPointerEvents Pointer events not delivered since the last frame, including a copy of the current event.
    /// \param predictedPointerEvents Predicted pointer events that will arrive between now and the next frame.
    /// \param estimatedProperties A set of estimated properties.
    /// \param estimatedPropertiesExpectingUpdates A set of estimated properties that are expecting updates.
    PointerEventArgs(const void* eventSource,
                     const std::string& eventType,
                     uint64_t timestampMicros,
                     uint64_t detail,
                     const Point& point,
                     std::size_t pointerId,
                     int64_t deviceId,
                     int64_t pointerIndex,
                     uint64_t sequenceIndex,
                     const std::string& deviceType,
                     bool isCoalesced,
                     bool isPredicted,
                     bool isPrimary,
                     int16_t button,
                     uint16_t buttons,
                     uint16_t modifiers,
                     const std::vector<PointerEventArgs>& coalescedPointerEvents,
                     const std::vector<PointerEventArgs>& predictedPointerEvents,
                     const std::set<std::string>& estimatedProperties,
                     const std::set<std::string>& estimatedPropertiesExpectingUpdates);


    /// \brief Destroy the pointer event args.
    virtual ~PointerEventArgs();

    /// \returns the Point data associated with this event.
    Point point() const;

    /// \brief Get the position of the event in screen coordinates.
    ///
    /// A convenience method equivalent to point().position();
    ///
    /// \returns the position in screen coordinates.
    glm::vec2 position() const;

    /// \brief Get a single unique id for a device id and Pointer index.
    /// \sa https://w3c.github.io/pointerevents/#dom-pointerevent-pointerid
    /// \returns a single unique id for a device id and Pointer index.
    std::size_t pointerId() const;

    /// \brief Get the unique input device id.
    /// \returns the unique input device id.
    int64_t deviceId() const;

    /// \brief Get the unique pointer index.
    ///
    /// This index should correspend to different touches for a multi-touch
    /// device.
    ///
    /// \returns the unique pointer index for the given device id or -1 if not supported.
    int64_t pointerIndex() const;

    /// \brief Get sequence index for this event.
    ///
    /// When supported it is a monotonically number. When unsupported, it is
    /// it is zero.
    ///
    /// \returns the sequence index for this event or -1 if not supported.
    uint64_t sequenceIndex() const;

//    /// \return a unique event key.
//    PointerKey pointerKey() const;

    /// \brief Get the device type string.
    ///
    /// This string may be TYPE_MOUSE, TYPE_TOUCH, TYPE_PEN, or a custom string.
    ///
    /// \returns a device description string.
    std::string deviceType() const;

    /// \returns true if the event was delivered as a coalesced event.
    bool isCoalesced() const;

    /// \returns true if this event was predicted rather than measured.
    bool isPredicted() const;

    /// \brief Determine if this pointer is the primary pointer.
    /// \returns true if this pointer is the primary pointer.
    /// \sa https://w3c.github.io/pointerevents/#the-primary-pointer
    bool isPrimary() const;

    /// \returns true if estimatedProperties().size() > 0.
    bool isEstimated() const;

    /// \brief Get the button id for this event.
    /// \returns the button id for this event.
    /// \todo This is not currently behaving to spec.
    /// \sa https://w3c.github.io/pointerevents/#the-button-property
    int16_t button() const;

    /// \brief Get all pressed buttons for this pointer.
    /// \return all pressed buttons for this pointer.
    /// \sa https://w3c.github.io/pointerevents/#button-states
    uint16_t buttons() const;

    /// \brief Get all modifiers for this pointer.
    /// \returns all modifiers for this pointer.
    uint16_t modifiers() const;

    /// \returns pointer events not delivered since the last frame, including a copy of the current event.
    std::vector<PointerEventArgs> coalescedPointerEvents() const;

    /// \returns predicted pointer events that will arrive between now and the next frame.
    std::vector<PointerEventArgs> predictedPointerEvents() const;

    /// \returns a set of estimated properties.
    std::set<std::string> estimatedProperties() const;

    /// \returns a set of estimated properties that are expecting updates.
    std::set<std::string> estimatedPropertiesExpectingUpdates() const;

    /// \brief Attempt to update properties with the given event.
    ///
    /// A property will be updated if:
    ///
    ///     - The sequence() of both events is the same.
    ///     - The updated property name is in the estimatedProperties() set.
    ///     - The updated property name is in the estimatedPropertiesExpectingUpdates() set.
    ///
    /// \returns true if this event was successfully updated.
    bool updateEstimatedPropertiesWithEvent(const PointerEventArgs& e);

    /// \brief Utility to convert ofTouchEventArgs events to PointerEventArgs.
    /// \todo Does not set "isPrimary" correctly since it has no context.
    /// \param source The event source.
    /// \param e The touch event to convert.
    /// \returns a PointerEventArgs.
    static PointerEventArgs toPointerEventArgs(const void* source,
                                               const ofTouchEventArgs& e);

    /// \brief Utility to convert ofTouchEventArgs events to PointerEventArgs.
    /// \todo Does not set "isPrimary" correctly since it has no context.
    /// \param e The touch event to convert.
    /// \param source The event source.
    /// \returns a PointerEventArgs.
    static PointerEventArgs toPointerEventArgs(const void* source,
                                               const ofMouseEventArgs& e);

    /// \brief A debug utility for viewing the contents of PointerEventArgs.
    /// \returns A string representation of the PointerEventArgs.
    std::string toString() const
    {
        std::stringstream ss;

        ss << "------------" << std::endl;
        ss << "     Source: " << eventSource() << std::endl;
        ss << "      Event: " << eventType() << std::endl;
        ss << "  Timestamp: " << timestampMillis() << std::endl;
        ss << " Pointer Id: " << pointerId() << std::endl;
        ss << "  Device Id: " << deviceId() << std::endl;
        ss << "Device Type: " << deviceType() << std::endl;
        ss << "     Button: " << button() << std::endl;
        ss << "    Buttons: " << ofToBinary(buttons()) << std::endl;
        ss << "  Modifiers: " << ofToBinary(modifiers()) << std::endl;
        ss << "Touch Index: " << pointerIndex() << std::endl;
        ss << "Sequence Id: " << sequenceIndex() << std::endl;

        return ss.str();
    }

    /// \brief An event that is called when a property is updated.
    ///
    /// The name of the property is sent with the event.
    ofEvent<std::string> pointerPropertyUpdate;

    /// \brief The mouse pointer type.
    static const std::string TYPE_MOUSE;

    /// \brief The pen pointer type.
    static const std::string TYPE_PEN;

    /// \brief The touch pointer type.
    static const std::string TYPE_TOUCH;

    /// \brief The unknown pointer type.
    static const std::string TYPE_UNKNOWN;

    /// \brief The pointer over event type.
    static const std::string POINTER_OVER;

    /// \brief The pointer enter event type.
    static const std::string POINTER_ENTER;

    /// \brief The pointer down event type.
    static const std::string POINTER_DOWN;

    /// \brief The pointer move event type.
    static const std::string POINTER_MOVE;

    /// \brief The pointer up event type.
    static const std::string POINTER_UP;

    /// \brief The pointer cancel event type.
    static const std::string POINTER_CANCEL;

    /// \brief The pointer update event type.
    static const std::string POINTER_UPDATE;

    /// \brief The pointer out event type.
    static const std::string POINTER_OUT;

    /// \brief The pointer leave event type.
    static const std::string POINTER_LEAVE;

    /// \brief The pointer scroll type.
    /// \todo This is not part of the PointerEvents spec.
    ///       It should be an extension of a mouse event.
    static const std::string POINTER_SCROLL;

    /// \brief The got pointer capture event type.
    static const std::string GOT_POINTER_CAPTURE;

    /// \brief The lost pointer capture event type.
    static const std::string LOST_POINTER_CAPTURE;

    /// \brief Property key for position.
    static const std::string PROPERTY_POSITION;

    /// \brief Property key for pressure.
    static const std::string PROPERTY_PRESSURE;

    /// \brief Property key for tilt x.
    static const std::string PROPERTY_TILT_X;

    /// \brief Property key for tilt y.
    static const std::string PROPERTY_TILT_Y;

    friend std::ostream& operator << (std::ostream& os, const PointerEventArgs& e);

private:
    /// \brief The location and orientation of the pointer.
    Point _point;

    /// \brief A unique pointer ID.
    ///
    /// This value must be unique from all other active pointers at any given
    /// time. Pointer indexes can be reused and are implementation specific.
    std::size_t _pointerId = 0;

    /// \brief The id of the device producing the pointer events.
    uint64_t _deviceId = 0;

    /// \brief The index of the pointer.
    int64_t _pointerIndex = 0;

    /// \brief The monotonically increasing sequence index for this event.
    uint64_t _sequenceIndex = 0;

    /// \brief The type of device that generated this Point.
    std::string _deviceType = TYPE_UNKNOWN;

    /// \brief Indicates if the event was delivered as a coalesced event.
    bool _isCoalesced = false;

    /// \brief Indicates if the event is predicted rather than measured.
    bool _isPredicted = false;

    /// \brief Indicates if the pointer is a primary pointer.
    ///
    /// In a multi-pointer (e.g. multi-touch) scenario, the isPrimary property
    /// is used to identify a master pointer amongst the set of active pointers
    /// for each pointer type. Only a primary pointer will produce compatibility
    /// mouse events. Authors who desire single-pointer interaction can achieve
    /// this by ignoring non-primary pointers.
    ///
    /// \sa http://www.w3.org/TR/pointerevents/#the-primary-pointer
    bool _isPrimary = false;

    /// \brief The current button associated with this event.
    int16_t _button = 0;

    /// \brief The current buttons being pressed.
    uint16_t _buttons = 0;

    /// \brief The current modifiers being pressed.
    uint16_t _modifiers = 0;

    /// \brief Pointer events not delivered since the last frame, including a copy of the current event.
    std::vector<PointerEventArgs> _coalescedPointerEvents;

    /// \brief Predicted pointer events that will arrive between now and the next frame.
    std::vector<PointerEventArgs> _predictedPointerEvents;

    /// \brief A set of estimated properties.
    std::set<std::string> _estimatedProperties;

    /// \param A set of estimated properties that are expecting updates.
    std::set<std::string> _estimatedPropertiesExpectingUpdates;

    friend class PointerEvents;

};


/// \brief Write a PointerEventArgs to a std::ostream.
/// \param os The std::ostream to write to.
/// \param e The event to write.
/// \returns a reference to the provided std::ostream.
inline std::ostream& operator << (std::ostream& os, const PointerEventArgs& e)
{
    os << e.toString();
    return os;
}



inline void to_json(nlohmann::json& j, const PointerEventArgs& v)
{
    j =
    {
        { "event_type", v.eventType() },
        { "timestamp_micros", v.timestampMicros() },
        { "detail", v.detail() },
        { "point", v.point() },
        { "pointer_id", v.pointerId() },
        { "device_id", v.deviceId() },
        { "pointer_index", v.pointerIndex() },
        { "sequence_index", v.sequenceIndex() },
        { "device_type", v.deviceType() },
        { "is_coalesced", v.isCoalesced() },
        { "is_predicted", v.isPredicted() },
        { "is_primary", v.isPrimary() },
        { "button", v.button() },
        { "buttons", v.buttons() },
        { "modifiers", v.modifiers() },
        { "coalesced_pointer_events", v.coalescedPointerEvents() },
        { "predicted_pointer_events", v.predictedPointerEvents() },
        { "estimated_properties", v.estimatedProperties() },
        { "estimated_properties_expecting_updates", v.estimatedPropertiesExpectingUpdates() },
    };
}


inline void from_json(const nlohmann::json& j, PointerEventArgs& v)
{
    v = PointerEventArgs(nullptr,
                         j.value("event_type", EventArgs::EVENT_TYPE_UNKNOWN),
                         j.value("timestamp_micros", uint64_t(0)),
                         j.value("detail", uint64_t(0)),
                         j.value("point", Point()),
                         j.value("pointer_id", std::size_t(0)),
                         j.value("device_id", uint64_t(0)),
                         j.value("pointer_index", uint64_t(0)),
                         j.value("sequence_index", uint64_t(0)),
                         j.value("device_type", PointerEventArgs::TYPE_UNKNOWN),
                         j.value("is_coalesced", false),
                         j.value("is_predicted", false),
                         j.value("is_primary", false),
                         j.value("button", int16_t(0)),
                         j.value("buttons", uint16_t(0)),
                         j.value("modifiers", uint16_t(0)),
                         j.value("coalesced_pointer_events", std::vector<PointerEventArgs>()),
                         j.value("predicted_pointer_events", std::vector<PointerEventArgs>()),
                         j.value("estimated_properties", std::set<std::string>()),
                         j.value("estimated_properties_expecting_updatess", std::set<std::string>()));
}


/// \brief A class for converting touch and mouse events into pointer events.
///
/// This class is a source of pointer events.  It captures mouse and touch
/// events from openFrameworks or an external source and repackages and
/// distributes them as pointer events.
///
/// This should not be accessed directly.
class PointerEvents
{
public:
//    enum EventSource
//    {
//        LEGACY_EVENTS,
//        EXTERNAL_EVENTS
//    };

    /// \brief Create a PointerEvents object with the given source.
    /// \param source The window that will provide the events.
    PointerEvents(ofAppBaseWindow* window);

    /// \brief Destroy the PointerEvents.
    ~PointerEvents();

    /// \brief Pointer event callback.
    /// \param source The event source.
    /// \param e the event arguments.
    /// \returns true of the event was consumed.
    bool onPointerEvent(const void* source, PointerEventArgs& e);

    /// \brief Mouse event callback.
    /// \param source The event source.
    /// \param e the event arguments.
    /// \returns true of the event was consumed.
    bool onMouseEvent(const void* source, ofMouseEventArgs& e);

    /// \brief Touch event callback.
    /// \param source The event source.
    /// \param e the event arguments.
    /// \returns true of the event was handled.
    bool onTouchEvent(const void* source, ofTouchEventArgs& e);

//    /// \brief Disable legacy mouse / touch events.
//    ///
//    /// If legacy mouse / touch events are disabled, they will be automatically
//    /// consumed if not already consumed by another callback.
//    ///
//    /// For legacy addons and other legacy user interface code, this should not
//    /// users should think carefully about disabling legacy events.
//    ///
//    /// Alternatively users that wish to use PointerEvents should simply not
//    /// register listeners for mouse or touch events.
//    void disableLegacyEvents();
//
//    /// \brief Enable legacy mouse / touch events.
//    ///
//    /// If legacy mouse / touch events are enabled, event propagation will not
//    /// artificially halted. If a consumer handles the event manually it will
//    /// not be propagated.
//    void enableLegacyEvents();

//    void setEnableLegacyEvents(bool enable);
//    bool getEnableLegacyEvents() const;

    /// \brief Register a pointer event listener.
    ///
    /// Event listeners registered via this function must have the following
    /// ofEvent callbacks defined:
    ///
    ///     `void onPointerDown(PointerEventArgs& evt)`
    ///     `void onPointerUp(PointerEventArgs& evt)`
    ///     `void onPointerMove(PointerEventArgs& evt)`
    ///     `void onPointerCancel(PointerEventArgs& evt)`
    ///
    /// Other method signatures event signatures are also supported.
    ///
    /// \tparam ListenerClass The class of the listener.
    /// \param listener A pointer to the listener class.
    /// \param prio The event priority.
    template <class ListenerClass>
    void registerPointerEvents(ListenerClass* listener,
                               int prio = OF_EVENT_ORDER_AFTER_APP);

    /// \brief Unregister a pointer event listener.
    ///
    /// Event listeners unregistered via this function must have the following
    /// ofEvent callbacks defined:
    ///
    ///     `void onPointerDown(PointerEventArgs& evt)`
    ///     `void onPointerUp(PointerEventArgs& evt)`
    ///     `void onPointerMove(PointerEventArgs& evt)`
    ///     `void onPointerCancel(PointerEventArgs& evt)`
    ///
    /// Other method signatures event signatures are also supported.
    ///
    /// \tparam ListenerClass The class of the listener.
    /// \param listener A pointer to the listener class.
    /// \param prio The event priority.
    template <class ListenerClass>
    void unregisterPointerEvents(ListenerClass* listener,
                                 int prio = OF_EVENT_ORDER_AFTER_APP);

    /// \brief Event that is triggered for any pointer event.
    ///
    /// All specific events below are triggered for matching events types if the
    /// event was not handled by an pointerEvent listener.
    ofEvent<PointerEventArgs> pointerEvent;

    /// \brief Event that is triggered when a point is introduced.
    ///
    /// Triggered after pointerEvent, if pointerEvent is not consumed.
    ofEvent<PointerEventArgs> pointerDown;

    /// \brief Event that is triggered when a point is removed.
    ///
    /// Triggered after pointerEvent, if pointerEvent is not consumed.
    ofEvent<PointerEventArgs> pointerUp;

    /// \brief Event that is triggered when a point moves.
    ///
    /// Triggered after pointerEvent, if pointerEvent is not consumed.
    ofEvent<PointerEventArgs> pointerMove;

    ///  \brief Event when the system cancels a pointer event.
    ///
    /// This event occurs when the pointer (touch or pen contact) is removed
    /// from the system. Here are common reasons why this might happen:
    ///     - A touch contact is CANCELLED by a pen coming into range of the
    ///     surface.
    ///     - The device doesn't report an active contact for more than 100ms.
    ///     - A mapping for a device's monitor changes while contacts are
    ///     active. For example, the user changes the position of a screen in a
    ///     two screen configuration.
    ///     - The desktop is locked or the user logged off.
    ///     - The number of simultaneous contacts exceeds the number that the
    ///     device can support. For example, if a device supports only two
    ///     contact points, if the user has two fingers on a surface, and then
    ///     touches it with a third finger, this event is raised.
    ofEvent<PointerEventArgs> pointerCancel;

    /// \brief Event that is triggered when a point has been updated.
    ///
    /// This event can be called in systems that offer updates to estimated
    /// pointer property data. For instance, iOS offers updates when using the
    /// iPencil. To update the point, the user can compare the
    ///
    ///     PointerEventArgs::estimatedProperties()
    ///
    /// Triggered after pointerEvent, if pointerEvent is not consumed.
    ofEvent<PointerEventArgs> pointerUpdate;

protected:
    /// \brief Dispatch the pointer events.
    /// \param source The event source.
    /// \param e the event arguments.
    /// \returns true of the event was handled.
    bool _dispatchPointerEvent(const void* source, PointerEventArgs& e);

    /// \brief True if the PointerEvents should consume mouse / touch events.
    bool _consumeLegacyEvents = false;

    /// \brief If true, the legacy events will be intercepted and converted to pointer events.
    bool _interceptLegacyEvents = true;

#if !defined(TARGET_OF_IOS) && !defined(TARGET_ANDROID)
    /// \brief Mouse moved event listener.
    ofEventListener _mouseMovedListener;

    /// \brief Mouse dragged event listener.
    ofEventListener _mouseDraggedListener;

    /// \brief Mouse pressed event listener.
    ofEventListener _mousePressedListener;

    /// \brief Mouse released event listener.
    ofEventListener _mouseReleasedListener;

    /// \brief Mouse scrolled event listener.
    ofEventListener _mouseScrolledListener;

    /// \brief Mouse entered the source window event listener.
    ofEventListener _mouseEnteredListener;

    /// \brief Mouse exited the source window event listener.
    ofEventListener _mouseExitedListener;
#endif

    /// \brief Touch downevent listener.
    ofEventListener _touchDownListener;

    /// \brief Touch up event listener.
    ofEventListener _touchUpListener;

    /// \brief Touch moved event listener.
    ofEventListener _touchMovedListener;

    /// \brief Touch double tap event listener.
    ofEventListener _touchDoubleTapListener;

    /// \brief Touch cancelled event listener.
    ofEventListener _touchCancelledListener;

    /// \brief The default source if the callback is missing.
    ofAppBaseWindow* _source = nullptr;

};


template <class ListenerClass>
void PointerEvents::registerPointerEvents(ListenerClass* listener, int prio)
{
    ofAddListener(pointerDown, listener, &ListenerClass::onPointerDown, prio);
    ofAddListener(pointerUp, listener, &ListenerClass::onPointerUp, prio);
    ofAddListener(pointerMove, listener, &ListenerClass::onPointerMove, prio);
    ofAddListener(pointerCancel, listener, &ListenerClass::onPointerCancel, prio);
}


template <class ListenerClass>
void PointerEvents::unregisterPointerEvents(ListenerClass* listener, int prio)
{
    ofRemoveListener(pointerDown, listener, &ListenerClass::onPointerDown, prio);
    ofRemoveListener(pointerUp, listener, &ListenerClass::onPointerUp, prio);
    ofRemoveListener(pointerMove, listener, &ListenerClass::onPointerMove, prio);
    ofRemoveListener(pointerCancel, listener, &ListenerClass::onPointerCancel, prio);
}


/// \brief Manages PointerEvents objects based on their ofAppBaseWindow source.
class PointerEventsManager
{
public:
    /// \returns a PointerEvents instance registered to listen to the global events or nullptr.
    PointerEvents* events();

    /// \returns a PointerEvents instance registered to listen to the given window or nullptr.
    PointerEvents* eventsForWindow(ofAppBaseWindow* window);

    /// \brief Get the singleton instance of the PointerEventsManager.
    /// \returns an instance of PointerEventsManager.
    static PointerEventsManager& instance();

private:
    /// \brief Create a default PointerEventsManager object.
    PointerEventsManager();

    /// \brief Destroy the PointerEventsManager.
    ~PointerEventsManager();

    /// \brief A map of windows to their pointer events.
    std::map<ofAppBaseWindow*, std::unique_ptr<PointerEvents>> _windowEventMap;

};


template <class ListenerClass>
void RegisterPointerEventsForWindow(ofAppBaseWindow* window, ListenerClass* listener, int prio = OF_EVENT_ORDER_AFTER_APP)
{
    PointerEvents* events = PointerEventsManager::instance().eventsForWindow(window);

    if (events)
    {
        events->registerPointerEvents(listener, prio);
    }
    else
    {
        ofLogError("RegisterPointerEventsForWindow") << "No PointerEvents available for given window.";
    }
}


template <class ListenerClass>
void UnregisterPointerEventsForWindow(ofAppBaseWindow* window, ListenerClass* listener, int prio = OF_EVENT_ORDER_AFTER_APP)
{
    PointerEvents* events = PointerEventsManager::instance().eventsForWindow(window);

    if (events)
    {
        events->unregisterPointerEvents(listener, prio);
    }
    else
    {
        ofLogError("UnregisterPointerEventsForWindow") << "No PointerEvents available for given window.";
    }
}


template <class ListenerClass>
void RegisterPointerEvents(ListenerClass* listener, int prio = OF_EVENT_ORDER_AFTER_APP)
{
    RegisterPointerEventsForWindow<ListenerClass>(ofGetWindowPtr(), listener, prio);
}


template <class ListenerClass>
void UnregisterPointerEvents(ListenerClass* listener, int prio = OF_EVENT_ORDER_AFTER_APP)
{
    UnregisterPointerEventsForWindow<ListenerClass>(ofGetWindowPtr(), listener, prio);
}


template <class ListenerClass>
void RegisterPointerEventForWindow(ofAppBaseWindow* window, ListenerClass* listener, int prio = OF_EVENT_ORDER_AFTER_APP)
{
    PointerEvents* events = PointerEventsManager::instance().eventsForWindow(window);

    if (events)
    {
        ofAddListener(events->pointerEvent, listener, &ListenerClass::onPointerEvent, prio);
    }
    else
    {
        ofLogError("RegisterPointerEventForWindow") << "No PointerEvents available for given window.";
    }
}


template <class ListenerClass>
void UnregisterPointerEventForWindow(ofAppBaseWindow* window, ListenerClass* listener, int prio = OF_EVENT_ORDER_AFTER_APP)
{
    PointerEvents* events = PointerEventsManager::instance().eventsForWindow(window);

    if (events)
    {
        ofRemoveListener(events->pointerEvent, listener, &ListenerClass::onPointerEvent, prio);
    }
    else
    {
        ofLogError("UnregisterPointerEventForWindow") << "No PointerEvents available for given window.";
    }
}



template <class ListenerClass>
void RegisterPointerEvent(ListenerClass* listener, int prio = OF_EVENT_ORDER_AFTER_APP)
{
    RegisterPointerEventForWindow<ListenerClass>(ofGetWindowPtr(), listener, prio);
}


template <class ListenerClass>
void UnregisterPointerEvent(ListenerClass* listener, int prio = OF_EVENT_ORDER_AFTER_APP)
{
    UnregisterPointerEventForWindow<ListenerClass>(ofGetWindowPtr(), listener, prio);
}


/// \brief A PointerStroke is a collection of events with the same pointer id.
///
/// A pointer stroke begins with a pointerdown event and ends with a pointerup
/// or pointercancel event.
class PointerStroke
{
public:
    /// \brief Construct a default PointerStroke.
    PointerStroke();

    /// \brief Destroy the PointerStroke.
    ~PointerStroke();

    /// \brief Add the given pointer event to the stroke.
    /// \returns true if the event was successfully added.
    bool add(const PointerEventArgs& e);

    /// \returns the PointerId of this PointerStroke.
    std::size_t pointerId() const;

    /// \returns the minimum sequence index.
    uint64_t minSequenceIndex() const;

    /// \returns the maximum sequence index.
    uint64_t maxSequenceIndex() const;

    /// \returns the minimum timestamp in microseconds.
    uint64_t minTimestampMicros() const;

    /// \returns the maximum timestamp in microseconds.
    uint64_t maxTimestampMicros() const;

    /// \returns true if the last event is a pointerup or pointercancel event.
    bool isFinished() const;

    /// \returns true if the last event is a pointercancel.
    bool isCancelled() const;

    /// \returns true if any events are still expecting updates.
    bool isExpectingUpdates() const;

    /// \returns the number of events.
    std::size_t size() const;

    /// \returns true if size() == 0.
    bool empty() const;

    /// \returns the events.
    const std::vector<PointerEventArgs>& events() const;

private:
    /// \brief The pointer id of all events in this stroke.
    std::size_t _pointerId = -1;

    /// \brief The minimum update sequence index.
    uint64_t _minSequenceIndex = std::numeric_limits<uint64_t>::max();

    /// \brief The maximum update sequence index.
    uint64_t _maxSequenceIndex = std::numeric_limits<uint64_t>::lowest();

    /// \brief All events associated with this stroke.
    std::vector<PointerEventArgs> _events;

};


/// \brief A utility class for visualizing Pointer events.
class PointerDebugRenderer
{
public:
    struct Settings;

    /// \brief Create a default debug renderer.
    PointerDebugRenderer();

    /// \brief Create a default debug renderer with the given settings.
    /// \p settings The settings values to set.
    PointerDebugRenderer(const Settings& settings);

    /// \brief Destroy the renderer.
    ~PointerDebugRenderer();

    /// \brief Configure the PointerDebugRenderer with the given settings.
    /// \p settings The settings values to set.
    void setup(const Settings& settings);

    /// \brief Update the strokes.
    ///
    /// This will remove strokes that have timed out.
    void update();

    /// \brief Draw the strokes.
    void draw() const;

    void draw(const PointerStroke& stroke) const;

    /// \brief Reset all data.
    void clear();

    /// \returns the Settings.
    Settings settings() const;

    /// \brief A callback for all Pointer Events.
    /// \param e The Pointer Event arguments.
    void add(const PointerEventArgs& e);

    // \returns the stroke map.
    const std::map<std::size_t, std::vector<PointerStroke>>& strokes() const;

    struct Settings
    {
        Settings();

        /// \brief Set the amount of time before the stroke is removed.
        uint64_t timeoutMillis = 5000;

        /// \brief The maximum width of the stroke in pixels.
        float strokeWidth = 100;

        /// \brief The color of normal points.
        ofColor pointColor;

        /// \brief The color of coalesced points.
        ofColor coalescedPointColor;

        /// \brief The color of predicted points.
        ofColor predictedPointColor;

    };

private:
    /// \brief The Settings.
    Settings _settings;

    /// \brief A map of strokes.
    std::map<std::size_t, std::vector<PointerStroke>> _strokes;

};


/// \brief A class for organiaing and querying collections of pointers.
class PointerEventCollection
{
public:
    /// \brief Create an empty PointerEventCollection.
    PointerEventCollection();

    /// \brief Destroy the PointerEventCollection.
    virtual ~PointerEventCollection();

    /// \returns the number of events in the collection.
    std::size_t size() const;

    /// \returns true if the size == 0.
    bool empty() const;

    /// \brief Clear all events in the collection.
    void clear();

    /// \returns the number of pointers currently tracked.
    std::size_t numPointers() const;

    /// \brief Determine if the PointerEventCollection has the given pointer event key.
    /// \param pointerEventKey The pointer event key to query.
    /// \returns true if the set already has the pointer event key.
    bool hasPointerId(std::size_t pointerId);

    /// \brief Add a pointer event to the set.
    /// \param pointerEvent The pointer event to add.
    void add(const PointerEventArgs& pointerEvent);

    /// \brief Remove all events for the given pointerId.
    /// \param pointerId The pointer events to remove.
    void removeEventsForPointerId(std::size_t pointerId);

    /// \returns all pointer events in the collection.
    std::vector<PointerEventArgs> events() const;

    /// \brief Get the pointer events for a given key.
    /// \param pointerId The pointer id to query.
    /// \returns the pointer events for the given key or an empty set if none.
    std::vector<PointerEventArgs> eventsForPointerId(std::size_t pointerId) const;

    /// \brief Get a pointer to the first event for a given pointer id.
    /// \param pointerId The pointer id to query.
    /// \returns a const pointer to the first event or nullptr if none.
    const PointerEventArgs* firstEventForPointerId(std::size_t pointerId) const;

    /// \brief Get a pointer to the last event for a given pointer id.
    /// \param pointerId The pointer id to query.
    /// \returns a const pointer to the last event or nullptr if none.
    const PointerEventArgs* lastEventForPointerId(std::size_t pointerId) const;

private:
    /// \brief The set of pointer events in order of addition.
    std::vector<PointerEventArgs> _events;

    /// \brief A mapping between the pointer event keys and the pointer events.
    std::map<std::size_t, std::vector<PointerEventArgs*>> _eventsForPointerId;

};


} // namespace ofx


/// \brief A hash function for combining std::hash<> values.
/// \param seed The hash to append to.
/// \param v The value to append.
/// \tparam T The class to hash. Must be compatible with std::hash<>.
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}
