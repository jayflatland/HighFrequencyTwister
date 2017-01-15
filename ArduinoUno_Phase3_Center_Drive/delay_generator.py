#!/usr/bin/env python




def genTable( key, accelMag, startSpd, minDelta, interdelay, note ):
    print "static const int delays90_%s[] = {" % key

    t = 0.0
    deg = 0.0
    spd = startSpd

    for i in range(50):
        deg += 1.8
        dt = 90.0/50.0/spd*1e6
        dt = max( dt, minDelta )
        print "    % 5d,   //% 5d deg/sec" % ( dt, spd )
        accel = accelMag
        if i >= 25:
            accel = -accelMag
        spd = spd + accel * dt / 1000.0
        t += dt

    print "};"
    print ""

    interdelayUS = interdelay*1000
    t90 = t + interdelayUS



    print "static const int delays180_%s[] = {" % key



    t = 0.0
    deg = 0.0
    spd = startSpd

    for i in range(100):
        deg += 1.8
        dt = 90.0/50.0/spd*1e6
        dt = max( dt, minDelta )
        print "    % 5d,   //% 5d deg/sec" % ( dt, spd )
        accel = accelMag
        if i >= 50:
            accel = -accelMag
        spd = spd + accel * dt / 1000.0
        t += dt

    print "};"
    print ""

    print "static int interdelay_%s = %s;" % ( key, interdelay )
    print ""


    t180 = t + interdelayUS
    print 'static inline void reportSpeedInfo_%s() {' % key
    print '    Serial.println( "" );'
    print '    Serial.println( "Speed %s - %s" );' % ( key, note )
    print '    Serial.println( "accelMag = %s" );' % accelMag
    print '    Serial.println( "startSpd = %s" );' % startSpd
    print '    Serial.println( "minDelta = %s" );' % minDelta
    print '    Serial.println( "interdelay = %s" );' % interdelay
    print '    Serial.println( " 90 turns - %dms each, %.2f for 25 moves" );' % ( t90  * 1e-3, t90  * 25 * 1e-6 )
    print '    Serial.println( "180 turns - %dms each, %.2f for 25 moves" );' % ( t180 * 1e-3, t180 * 25 * 1e-6 )
    print '}'
    print ''
    print ''


tables = [
    #   key     accel       startSpd    minDelta    interdelay (ms)     note
    (   "1",     80.0,       800.0,     500,         0,                 "safe"          ),
    #(   "2",    100.0,      1200.0,     500,         0,                 "fast"          ),
    (   "3",    120.0,      1200.0,     500,         0,                 "faster"        ),
    (   "4",    122.0,      1200.0,     400,         0,                 "ludicrous"     ),
    (   "5",    130.0,      1250.0,     350,         0,                 "ludicrouser"   ),#pretty stable
    #(   "5",    140.0,      1300.0,     300,         0,                 "ludicrouser"   ),#somewhat unstable

    #(   "5",    120.0,      1200.0,     400,         0,                 "test A+2"      ),
    #(   "6",    130.0,      1200.0,     400,         0,                 "test A+1"      ),
    #(   "7",    140.0,      1200.0,     400,         0,                 "test A+2"      ),
    #(   "8",    150.0,      1200.0,     400,         0,                 "test A+3"      ),
    #(   "9",    160.0,      1200.0,     400,         0,                 "test A+4"      ),
]



for key, accelMag, startSpd, minDelta, interdelay, note in tables:
    genTable( key, accelMag, startSpd, minDelta, interdelay, note )


print 'static inline void reportSpeedInfo() {'
print '    switch( speed ) {'
print '    default:'
for key, accelMag, startSpd, minDelta, interdelay, note in tables:
    print '    case %s: reportSpeedInfo_%s(); break;' % ( key, key )
print '    }'
print '}'
print ''
print 'static inline int getInterDelay() {'
print '    switch( speed ) {'
print '    default:'
for key, accelMag, startSpd, minDelta, interdelay, note in tables:
    print '    case %s: return interdelay_%s;' % ( key, key )
print '    }'
print '}'
print ''
print 'static inline int getDelay90( int i ) {'
print '    switch( speed ) {'
print '    default:'
for key, accelMag, startSpd, minDelta, interdelay, note in tables:
    print '    case %s: return delays90_%s[ i ];' % ( key, key )
print '    }'
print '}'
print ''
print 'static inline int getDelay180( int i ) {'
print '    switch( speed ) {'
print '    default:'
for key, accelMag, startSpd, minDelta, interdelay, note in tables:
    print '    case %s: return delays180_%s[ i ];' % ( key, key )
print '    }'
print '}'
