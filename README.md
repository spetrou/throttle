# Throttle
C++ event throttle mechanism.

### Motivation
I was recently involved in implementing a [FIX](http://www.fixprotocol.org/) message client.
One of the issues we wanted to avoid is client applications going crazy and send
massive volumes of incoming messages to our platform. To avoid such a risk we needed a
throttle mechanism to limit the number of incoming messages we process per client. Also
we wanted an alarm to be triggered when a connection was misbehaving. By *misbehaving* here
I mean sending too many FIX messages.

### Description
The function of `class Throttle` is to maintain a *rate* of events for a given *window*.
For example 10 events every second. A call to `waitTurn` creates a new *event*
and checks if the *rate* was reached. If it has, it waits the appropriate amount of time
before returning. The call returns the number of milliseconds it needed to wait. When
the rate was not reached, 0 is returned.

### Implementation Details
The `class Throttle` keeps a internal list of *events*. Each *event* is represented
as a timestamp (for general knowledge the timestamps are UTC with high precision,
millisecond and higher depending on the platform). Each call to `waitTurn` appends
an event to the list. Any expired events are removed from the list before the rate
check.

### Library Requirements
The mechamism uses two third party libraries:

* [log4cpp](http://log4cpp.sourceforge.net/) - Used for file logging.
* [boost](http://www.boost.org/) - Used for high precision timestamp
{package [date\_time](www.boost.org/libs/date_time/)} and sleep
{package [thread](www.boost.org/libs/thread/)}.

