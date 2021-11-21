#include <stdio.h>
#include <iostream>
#include <memory>


//--------------- General definition ----------------

// declaration of a builder
class sdpProcessingFunctionBuilder;

// declaration and definition of the processing function
class sdpProcessingFunction {
    friend class sdpProcessingFunctionBuilder;

public: // definition of configuration for use outside of processing function
    struct sdpProcessingFunctionConfig {
        int a;
        int b;
        int use_gpu; // ideally what we need is custom implementation of shared_ptr which would keep track of where the pointer lives (host, device) and/or where is the most current version of the data
    };

private:
    sdpProcessingFunctionConfig config;
    int kernel_to_use;

    int *temporary_workarea;

    sdpProcessingFunction(sdpProcessingFunctionConfig outside_config) {
        config = outside_config;
    }

public:
    // method which checks if the configuration is correct
    static bool check(sdpProcessingFunctionConfig &config) {
        if (config.a==0) return false;
        if (config.b==0) return false;
        return true;
    }

    void run() {
        // when running the PF everything is configured and checked with resources allocated
        // these are primitive examples but for example doing FFT may require different amount of
        // temporary workarea based on parameters, stride, or memory arrangement. 
        if (check(this->config)) {
            if (config.use_gpu==1) {
                printf("Doing stuff on GPU...\n");
            }
            else {
                printf("Doing stuff...\n");
            }
            if (kernel_to_use==2) {
                printf("Using PF for b>5...\n");
                printf("Accessing allocated temporary workspace: %d;\n", temporary_workarea[0]);
            }
            else if (kernel_to_use==1) {
                printf("Using PF for 0<b<5...\n");
            }
            printf("Value of the variable a is %d;\n", config.a);
            printf("Value of the variable b is %d;\n", config.b);
            printf("Done!\n");
        }
        else {
            printf("Something is WRONG! We should not be able to get to this!\n");
        }
    };

    ~sdpProcessingFunction() {
        if (temporary_workarea!=nullptr) {
            delete temporary_workarea;
            temporary_workarea = nullptr;
        }
    }
};


class sdpProcessingFunctionBuilder {
public: // variables
    sdpProcessingFunction::sdpProcessingFunctionConfig config;

public: // methods
    sdpProcessingFunctionBuilder() {
        config.a = 0;
        config.b = 0;
        config.use_gpu = 0;
    }

    // build returns instance of the processing function
    sdpProcessingFunction *build() {
        if (sdpProcessingFunction::check(config)) {
            sdpProcessingFunction *temp = new sdpProcessingFunction(config);
            temp->temporary_workarea = nullptr;
            if (config.b>5) {
                temp->kernel_to_use = 2;
                temp->temporary_workarea = new int[10];
                temp->temporary_workarea[0] = 1024;
            }
            else temp->kernel_to_use = 1;
            return(temp);
        }
        else {
            throw new std::exception;
        }
    }

    // using smart pointers may be preferred.
    // ideally what we need is custom implementation of shared_ptr which would keep track of where the pointer lives (host, device) and/or where is the most current version of the data
    // only one build function should be used two of these are to illustrate options 
    std::shared_ptr<sdpProcessingFunction> buildSharedPtr() {
        if (sdpProcessingFunction::check(config)) {
            std::shared_ptr<sdpProcessingFunction> temp(new sdpProcessingFunction(config));
            temp->temporary_workarea = nullptr;
            if (config.b>5) {
                temp->kernel_to_use = 2;
                temp->temporary_workarea = new int[10];
                temp->temporary_workarea[0] = 1024;
            } else temp->kernel_to_use = 1;
            return(temp);
        } else {
            throw new std::exception;
            return nullptr;
        }
    }

    sdpProcessingFunctionBuilder& fromFile() {
        printf("Pretending to loading a file and configure based on that\n");
        config.a = 5;
        config.b = 6;
        return *this;
    };

    sdpProcessingFunctionBuilder& fromConfiguration(sdpProcessingFunction::sdpProcessingFunctionConfig &ref_config) {
        config = ref_config;
        return *this;
    }
};


int main() {
    printf("Starting...\n");
    printf("\n");

    // Creating some configuration
    printf("Create a configuration.\n");
    sdpProcessingFunction::sdpProcessingFunctionConfig some_config;
    printf("Check configuration's validity\n");
    some_config.a = 0;
    some_config.b = 0;
    if (sdpProcessingFunction::check(some_config)) printf("Configuration is valid!\n"); else printf("Configuration is invalid!\n");
    printf("Make the configuration valid...\n");
    some_config.a = 2;
    some_config.b = 3;
    if (sdpProcessingFunction::check(some_config)) printf("Configuration is valid!\n"); else printf("Configuration is invalid!\n");
    printf("Done with configuration.\n");
    printf("\n");

    // Creating an instance of processing function
    // sdpProcessingFunction sdpPrFunc(some_config); // Does not work because constructor is private
    
    // We must use builder to get an instance of sdpProcessingFunction
    sdpProcessingFunctionBuilder sdp_builder;
    printf("Now builder has invalid configuration so it returns nullptr if used.\n");
    sdpProcessingFunction *sdpPrFunc_wrong = nullptr;
    std::shared_ptr<sdpProcessingFunction> sdpPrFunc_wrong_shared = nullptr;
    try { sdpPrFunc_wrong = sdp_builder.build(); }
    catch (...) { 
        printf("Bad configuration creation of the processing function failed!\n");
        sdpPrFunc_wrong = nullptr;
    }
    try { sdpPrFunc_wrong_shared = sdp_builder.buildSharedPtr(); }
    catch (...) { 
        printf("Bad configuration creation of the processing function failed with shared_ptr!\n"); 
        sdpPrFunc_wrong_shared = nullptr;
    }
    if (sdpPrFunc_wrong==nullptr) printf("Sure enough sdpPrFunc is nullptr!\n");
    if (sdpPrFunc_wrong_shared==nullptr) printf("Sure enough sdpPrFunc is nullptr!\n");

    printf("\n");
    
    printf("We can configure our function with a file...\n");
    sdp_builder.fromFile();
    sdpProcessingFunction *sdpPrFunc_file;
    std::shared_ptr<sdpProcessingFunction> sdpPrFunc_file_shared;
    try { sdpPrFunc_file = sdp_builder.build(); }
    catch (...) {
        printf("Bad configuration creation of the processing function failed!\n");
        sdpPrFunc_file = nullptr;
    }
    try { sdpPrFunc_file_shared = sdp_builder.buildSharedPtr(); }
    catch (...) {
        printf("Bad configuration creation of the processing function failed with shared_ptr!\n");
        sdpPrFunc_file_shared = nullptr;
    }
    // now processing function is valid (pointer exists)
    if (sdpPrFunc_file==nullptr) printf("Sure enough sdpPrFunc is nullptr!\n");
    if (sdpPrFunc_file_shared==nullptr) printf("Sure enough sdpPrFunc is nullptr!\n");
    printf("Running our processing function configured from file gives:\n");
    sdpPrFunc_file->run();
    printf("and now with shared_ptr:\n");
    sdpPrFunc_file_shared->run();
    
    printf("\n");

    printf("We can change the configuration directly to invalid configuration...\n");
    sdp_builder.config.a = 0;
    sdp_builder.config.b = 0;
    printf("and create an instance of processing function which will be nullptr again.\n");
    sdpProcessingFunction *sdpPrFunc_wrong_again;
    std::shared_ptr<sdpProcessingFunction> sdpPrFunc_wrong_again_shared;
    try { sdpPrFunc_wrong_again = sdp_builder.build(); }
    catch (...) {
        printf("Bad configuration creation of the processing function failed!\n");
        sdpPrFunc_wrong_again = nullptr;
    }
    try { sdpPrFunc_wrong_again_shared = sdp_builder.buildSharedPtr(); }
    catch (...) {
        printf("Bad configuration creation of the processing function failed with shared_ptr!\n");
        sdpPrFunc_wrong_again_shared = nullptr;
    }
    if (sdpPrFunc_wrong_again==nullptr) printf("Sure enough sdpPrFunc is nullptr!\n");
    if (sdpPrFunc_wrong_again_shared==nullptr) printf("Sure enough sdpPrFunc is nullptr!\n");

    printf("\n");

    printf("Or use previously defined configuration...\n");
    sdp_builder.fromConfiguration(some_config);
    sdpProcessingFunction *sdpPrFunc_config = sdp_builder.build();
    if (sdpPrFunc_config==nullptr) printf("Sure enough sdpPrFunc is nullptr!\n");
    printf("which is valid and running it gives:\n");
    sdpPrFunc_config->run();

    printf("\n");

    printf("We can also chain operations...\n");
    std::shared_ptr<sdpProcessingFunction> sdpPrFunc_config_chain_shared = sdp_builder.fromConfiguration(some_config).buildSharedPtr();
    sdpPrFunc_config_chain_shared->run();



    delete sdpPrFunc_wrong;
    delete sdpPrFunc_file;
    delete sdpPrFunc_wrong_again;
    delete sdpPrFunc_config;

}
