#include <iostream>
#include <memory>

class pfVectorAdditionBuilder;

class pfVectorAddition {
    friend class pfVectorAdditionBuilder;

public: // definitions
    struct pfVectorAdditionConfig {
        int vector_length;
        int nVectors;
        bool use_gpu;
    };

private:
    pfVectorAdditionConfig config;

    pfVectorAddition(pfVectorAdditionConfig outside_config) {
        config = outside_config;
    }

public:
    static bool check(pfVectorAdditionConfig &config) {
        if (config.vector_length < 1) return false;
        if (config.nVectors < 1) return false;
        return true;
    }

    void vectorAdditionGpu(float *output, float *A, float *B) {
        printf("GPU\n");
    }

    void vectorAddition(float *output, float *A, float *B) {
        size_t nIterations = config.vector_length*config.nVectors;
        for (size_t f = 0; f<nIterations; f++) {
            output[f] = A[f] + B[f];
        }
    }

    void run(float *output, float *A, float *B) {
        if (check(this->config)) {
            if (config.use_gpu==true) {
                vectorAdditionGpu(output, A, B);
            } else {
                vectorAddition(output, A, B);
            }
        } else {
            printf("Something is WRONG! We should not be able to get to this!\n");
        }
    };
};


class pfVectorAdditionBuilder {
public: // variables
    pfVectorAddition::pfVectorAdditionConfig config;

public: // methods
    pfVectorAdditionBuilder() {
        config.vector_length = 0;
        config.nVectors = 0;
        config.use_gpu = false;
    }

    std::shared_ptr<pfVectorAddition> buildSharedPtr() {
        if (pfVectorAddition::check(config)) {
            std::shared_ptr<pfVectorAddition> temp(new pfVectorAddition(config));
            return(temp);
        } else {
            throw new std::exception;
            return nullptr;
        }
    }

    pfVectorAdditionBuilder &fromFile() {
        // loading from file
        return *this;
    };

    pfVectorAdditionBuilder &fromConfiguration(pfVectorAddition::pfVectorAdditionConfig &ref_config) {
        config = ref_config;
        return *this;
    }
};



int main() {
    printf("Starting...\n");
    printf("\n");

    // Vector Addition
    int nVectors = 100000;
    int vector_length = 3;
    size_t size = nVectors*vector_length;
    float *output, *A, *B;
    output = new float[size];
    A = new float[size];
    B = new float[size];
    for (size_t f = 0; f<size; f++) {
        output[f] = 0;
        A[f] = f%5;
        B[f] = f%11;
    }
    pfVectorAdditionBuilder VABuilder;
    VABuilder.config.nVectors = nVectors;
    VABuilder.config.vector_length = vector_length;
    std::shared_ptr<pfVectorAddition> pfVA;
    try { pfVA = VABuilder.buildSharedPtr(); }
    catch (...) {
        printf("Instantiation of processing function failed!\n");
        pfVA = nullptr;
        return 1;
    }
    pfVA->run(output, A, B);
    for (int f = 0; f<10; f++) {
        for (int i = 0; i<vector_length; i++) {
            int pos = f*vector_length + i;
            printf("%f + %f = %f\n", A[pos], B[pos], output[pos]);
        }
        printf("\n");
    }

}
