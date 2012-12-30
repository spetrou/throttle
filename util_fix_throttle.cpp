
#include "util_fix_throttle.h"
#include <unistd.h>
#include <boost/thread/thread.hpp>

namespace Util {
namespace Fix {    

using namespace boost;

std::ostream& operator<<(std::ostream& os, const Throttle& obj)
{
    os << "Events in queue: [" << obj.d_ticks.size() << "]. "
       << "Monitor window: [" << obj.d_monitorWindow << "]. "
       << "Rate of events: [" << obj.d_rate << "].";
    return os;
}

Throttle::Throttle(log4cpp::Category* logger)
: d_rate(1)
, d_logger(logger)
, d_monitorWindow(posix_time::seconds(1))
{ }

Throttle::Throttle(const unsigned int& rate,
                   log4cpp::Category* logger)
: d_rate(rate)
, d_monitorWindow(posix_time::seconds(1))
, d_logger(logger)
{
    // Check that the rate is not zero
    if(!d_rate) {
        std::string msg("The rate of the Throttle cannot be zero. Re-setting to 1.");
        if(d_logger) { d_logger->info(msg); }
        else { std::cerr << msg << std::endl; }
    }
}

Throttle::Throttle(const unsigned int& rate,
                   const boost::posix_time::time_duration& window,
                   log4cpp::Category* logger)
: d_rate(rate)
, d_monitorWindow(window)
, d_logger(logger)
{ }


Throttle::~Throttle(void) { }

void Throttle::setRate(const unsigned int& rate)
{
    // Check that the rate is not zero
    if(!rate) {
        std::string msg("The rate of the Throttle cannot be zero.");
        if(d_logger) { d_logger->info(msg); }
        else { std::cerr << msg << std::endl; }
    } else {
        d_rate = rate;
    }
}

unsigned int Throttle::getRate(void) const
{
    return d_rate;
}

void Throttle::expireTicks(const boost::posix_time::ptime& now)
{
    while(d_ticks.size()) {
        if((now - d_ticks.front()) >= d_monitorWindow) {
            if(d_logger) {
                d_logger->debugStream() << "Expiring tick: ["
                                        << d_ticks.front() << "]"
                                        << log4cpp::eol;
            }
            d_ticks.pop_front();
        } else {
            break;
        }
    }
}

int Throttle::waitTurn(void)
{
    posix_time::ptime now(posix_time::microsec_clock::universal_time());

    this->expireTicks(now);

    d_ticks.push_back(now);

    // ***NOTE***: Size here will always be greater than 1 since it's
    //             right after the "push_back". So we need the greater
    //             or equal below to work correctly.
    if(d_ticks.size() <= d_rate) {
        return 0;
    }

    // ***CAUTION***: We are taking a risk for performance here.
    //                The following code is called always *AFTER* a new element
    //                is added to the deque. So we skip the size check because
    //                there's always at least one element. Below we access the
    //                first element of the container without checking
    //                the size of the container.
    const posix_time::ptime& last = d_ticks.front();

    this_thread::sleep(now-last);

    if(d_logger) {
        d_logger->infoStream() << "Waited ["
                               << (now - last) << "]"
                               << ",\t elements: [" << d_ticks.size() << "]"
                               << log4cpp::eol;
    }

    return (now - last).total_milliseconds();
}

} // end of Fix
} // end of Util

