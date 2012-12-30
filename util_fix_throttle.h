#ifndef INCLUDE_UTIL_FIX_THROTTLE_H
#define INCLUDE_UTIL_FIX_THROTTLE_H

#include <deque>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <log4cpp/Category.hh>

namespace Util {
namespace Fix {

class Throttle {
private:
    friend std::ostream& operator<<(std::ostream&, const Throttle&);

    std::deque<boost::posix_time::ptime> d_ticks;
    unsigned int d_rate;
    boost::posix_time::time_duration d_monitorWindow;
    log4cpp::Category* d_logger;

    // Lock Copy and Assignment constructors
    Throttle(const Throttle& other);
    Throttle operator=(const Throttle&);

    /*!
     *  @brief      Expire ticks.
     *  @details    Remove from the internal queue the ticks that
     *              are outside the monitor window. The ptime of
     *              "now" is passed in.
     *  @param  now     An instance of ptime with the currect universal
     *                  time.
     */
    void expireTicks(const boost::posix_time::ptime& now);

public:

    /*!
     *  @brief      Default constructor.
     *  @details    The default constructor. The rate is by default
     *              set to 1 event every second.
     *  @param  logger  A log4cpp logger pointer. NULL by default.
     */
    Throttle(log4cpp::Category* logger=NULL);

    /*!
     *  @brief      Construct object with a rate.
     *  @details    Create a thottler with the given rate.
     *  @param  rate    The rate to set.
     *  @param  window  The monitor window duration.
     *  @param  logger  A log4cpp logger pointer. NULL by default.
     */
    explicit Throttle(const unsigned int& rate,
                      const boost::posix_time::time_duration& window,
                      log4cpp::Category* logger=NULL);

    /*!
     *  @brief      Construct object with a rate and a monitor window.
     *  @details    Create a thottler with the given rate of events and
     *              monitor window.
     *  @param  rate    The rate to set.
     *  @param  logger  A log4cpp logger pointer. NULL by default.
     */
    explicit Throttle(const unsigned int& rate,
                      log4cpp::Category* logger=NULL);

    /*!
     *  @brief      Default Destructor.
     *  @details    The default desctuctor. Declared virtual to
     *              accomodate any inheritance.
     */
    virtual ~Throttle(void);

    /*!
     *  @brief      Check the rate and sleep if needed.
     *  @details    Check how many events occured during the time
     *              window and sleep the amount of time needed.
     *              Return the amount of time needed to sleep
     *              (if any) in milliseconds.
     *  @return How many milliseconds it needed to wait.
     */
    int waitTurn(void);

    /*!
     *  @brief      Set throttle rate.
     *  @details    Set the value of the internal rate.
     *  @param  rate    The value of the rate to set.
     */
    void setRate(const unsigned int& rate);

    /*!
     *  @brief      Get throttle rate.
     *  @details    Get the value of the internal rate.
     *  @return The integer value of the internal rate.
     */
    unsigned int getRate(void) const;
};

} // end of Fix
} // end of Util

#endif

