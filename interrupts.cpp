/**
 *
 * @file interrupts.cpp
 * @author Ali Asghar Bundookwalla (101299213)
 * @author Mohamed Gomaa (101309418)
 */

#include <interrupts.hpp>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    //Timer of OS
    int current_time = 0;

    //Track kernel mode (REMOVE LATER IF NOT USED)
    bool kernel_mode = false;

    // Constants
    int context_save_time = 10;
    int isr_activity_time = 40;
    int negligible_time = 1;

    // Device tracking (REMOVE LATER IF NOT USED)
    std::vector<bool> device_busy;
    std::vector<int> device_completion_time;
    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        if (activity == "CPU"){
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU Burst\n";
            current_time += duration_intr;
        }
        else if(activity == "SYSCALL" || activity == "END_IO"){
            auto[execution_return, current_time_return] = intr_boilerplate(current_time, duration_intr, context_save_time, negligible_time ,vectors);
            execution += execution_return;
            current_time = current_time_return;

            // Execute ISR Body
            int remaining_time = delays.at(duration_intr);

            while (remaining_time >= isr_activity_time) {
                execution += std::to_string(current_time) + ", " + std::to_string(isr_activity_time) + ", " + activity + ": ran ISR Activity for device " + std::to_string(duration_intr + 1) + "\n";
                current_time += isr_activity_time;
                remaining_time -= isr_activity_time;
            }

            if (remaining_time > 0){
                execution += std::to_string(current_time) + ", " + std::to_string(remaining_time) + ", " + activity + ": remaining delay for device " + std::to_string(duration_intr + 1) + "\n";
                current_time += remaining_time;
            }
        
            // Execute IRET
            execution += std::to_string(current_time) + ", " + std::to_string(negligible_time) + ", IRET Execution\n"; //Switch Back to User Mode
            current_time += negligible_time;
            
        }

        else if(activity == "END_IO"){
            auto[execution_return, current_time_return] = intr_boilerplate(current_time, duration_intr, context_save_time, negligible_time ,vectors);
            execution += execution_return;
            current_time = current_time_return;

            // Execute ISR Body
            int remaining_time = 0;
            for (int i = delays.at(duration_intr); i<isr_activity_time; i-=isr_activity_time){ // Loop to simulate multiple lines in ISR body if needed
                execution += std::to_string(current_time) + ", " + std::to_string(delays.at(duration_intr)) + ", END_IO : ran ISR Execution for device " + std::to_string(duration_intr) + "\n";
                current_time += isr_activity_time;
                if (i< isr_activity_time){
                    remaining_time = i;
                }
            }

            if (remaining_time > 0){
                execution += std::to_string(current_time) + ", " + std::to_string(remaining_time) + ", END_IO : remaining delay for device " + std::to_string(duration_intr) + "\n";
                current_time += remaining_time;
            }
        
            // Execute IRET
            execution += std::to_string(current_time) + ", " + std::to_string(negligible_time) + ", IRET Execution\n"; //Switch Back to User Mode
            current_time += negligible_time;
        }


        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
