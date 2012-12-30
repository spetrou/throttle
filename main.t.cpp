
#include <iostream>
#include <unistd.h>

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/PatternLayout.hh>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "util_fix_throttle.h"

/*!
 *  @brief      Create a log4cpp logger.
 *  @details    Create and initialize a file based logger.
 *  @return The created logger.
 */
log4cpp::Category& createLogger(void);

/*!
 *  @brief      Test the throttler with light load events.
 *  @details    Create and schedule light load events to make
 *              sure that the throttler waits the right amount
 *              of time before executing the next event. Usual
 *              behavior in this test is to run many events at
 *              the beginning of the monitor window and then
 *              sleep for the rest of the duration.
 *  @param  logger  Log4cpp logger object.
 */
void lightLoadTest(log4cpp::Category& logger);

/*!
 *  @brief      Create fake CPU logs.
 *  @details    Consume CPU cycles by executing a double "for" loop.
 *  @param  a   Outer loop count. Affect the load by big factor.
 *  @param  b   Inner loop count. Affect the load by smaller factor.
 */
void fakeLoad(const int a, const int b);

int main(int argc, char** argv)
{
    log4cpp::Category& logger = createLogger();

    lightLoadTest(logger);

    log4cpp::Category::shutdown();

    return EXIT_SUCCESS;
}


log4cpp::Category& createLogger(void)
{
    /*Setting up Appender, layout and Category*/
    log4cpp::Appender *appender = new log4cpp::FileAppender("FileAppender", "./throttle_log");
    log4cpp::Layout *myLayout = new log4cpp::PatternLayout();
    ((log4cpp::PatternLayout*) myLayout)->setConversionPattern("%d %-5p [[%c]] - %m%n");
    log4cpp::Category& category = log4cpp::Category::getInstance("Throttler");

    appender->setLayout(myLayout);
    category.setAppender(appender);
    category.setPriority(log4cpp::Priority::INFO);

    return category;
}

void lightLoadTest(log4cpp::Category& logger)
{
    using namespace boost;

    Util::Fix::Throttle myThrottler(10, &logger);
//    Util::Fix::Throttle myThrottler(10,
//                                    posix_time::time_duration(posix_time::seconds(2)),
//                                    &logger);
    logger.infoStream() << myThrottler << log4cpp::eol;

    logger.info("Starting lightLoadTest...");
    clock_t start = clock();
    posix_time::ptime pStart(posix_time::microsec_clock::universal_time());

    for(int i=0; i < 20; ++i) { myThrottler.waitTurn(); fakeLoad(1000, 100); }
    for(int i=0; i < 20; ++i) { myThrottler.waitTurn(); fakeLoad(1000, 100); }
    for(int i=0; i < 10; ++i) { myThrottler.waitTurn(); fakeLoad(1000, 100); }
    for(int i=0; i < 10; ++i) { myThrottler.waitTurn(); fakeLoad(1500, 100); }
    clock_t stop = clock();
    posix_time::ptime pStop(posix_time::microsec_clock::universal_time());
    logger.infoStream() << "Events: [60]. Clock cycles: ["
                        << (stop - start) << "]"
                        << ". Duration: [" << (pStop - pStart) << "]"
                        << log4cpp::eol;
}

void fakeLoad(const int a, const int b)
{
    double r = 0;
    for ( int i = 0 ; i < a ; i++ ) {
        for ( int j = 0 ; j < b ; j++ ) {
            r = r + i * j;
        }
    }
}

