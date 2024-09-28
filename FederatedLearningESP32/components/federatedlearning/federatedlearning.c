#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "federatedlearning.h"
#include "trainingconfigs.h"

#include "esp_log.h"
#include "esp_heap_caps.h"

//////////////////////////////////////////////////PRINT//////////////////////////////////////////////////

void teste(){
    printf("🐉\n");
}

void PrintNeuralNetwork(NeuralNetwork * neuralnetwork) {

  int i = 0, j = 0, k = 0;

  Layer * currentlayer = neuralnetwork -> firstlayer;
  while (currentlayer != NULL) {

    i++;
    printf("layer %d neurons: %d\n", i, currentlayer -> neurons);

    Neuron * currentneuron = currentlayer -> firstneuron;

    while (currentneuron != NULL) {

      j++;
      printf("%s neuron %d - weights: %d\n",currentneuron->neurontype , j,currentneuron -> weights);
      printf("Bias: %.10f\n",currentneuron->bias);

      Weight * currentweight = currentneuron -> firstweight;

      while (currentweight != NULL) {

        k++;
        printf("%d value: %.10f ", k, currentweight -> weight);

        currentweight = currentweight -> nextweight;
      }
      printf("\n");
      k = 0;
      currentneuron = currentneuron -> nextneuron;
    }
    j = 0;
    currentlayer = currentlayer -> nextlayer;

  }
  i = 0;
  printf("\n");

}

void printVector(float *data,int size){
  for(int i =0; i< size;i++){
    printf("%f ", data[i]);
  }
  printf("\n");
}

void printMatriz(float **data, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%f ", data[i][j]);
        }
        printf("\n");
    }
}

//////////////////////////////////////////////////DEEPLEARNINGMATHFUNCTIONS//////////////////////////////////////////////////

float Perceptron(float Z){
    if(Z>0)
    {
      return 1;
    }
    else{
     return 0; 
    }
}

float PerceptronDerivative(float a){
    if(a>0){
      return 1;
    }
    else{
     return 0; 
    }
}

float ReLU(float Z){
  if(Z>0){
    return Z;
  }else{
    return 0;
  }
}

float ReLUDerivative(float a){
    if(a>0){
      return 1;
    }
    else{
     return 0; 
    }
}

float Sigmoid(float Z) {
  return 1.0 / (1.0 + exp(-Z));
}

float SigmoidDerivative(float a) {
  return a * (1 - a);
}

float SoftMax( float Z,float softmaxsum ){
  return exp(Z)/softmaxsum;
}

float SoftMaxDerivative1(float a){
  
  return a*(1-a);
}

float SoftMaxDerivative2(float a1 , float a2){
  
  return a1*a2;

}

float CategoricalCrossEntropyDerivative(float y, float a) {
  return -(y / a);
}

float CategoricalCrossEntropy(float * label, float * output, int labelsize) {
  float sum = 0;
  for (int i = 0; i < labelsize; i++) {
    sum += label[i] * log(output[i]);
  }
  return -sum;
}

//////////////////////////////////////////////////FREEALLOCATEDMEMORY//////////////////////////////////////////////////

void freeWeight(Weight *weight) {
    if(weight!=NULL){
        free(weight);        
    }
}

void freeNeuron(Neuron *neuron) {
    if(neuron!=NULL){
        free(neuron);        
    }
}

void freeLayer(Layer *layer) {
    if(layer!=NULL){
        free(layer);        
    }
}

void freeNeuralNetwork(NeuralNetwork neuralnetwork) {
      //printf("teste 0");
    
    Layer *currentlayer = neuralnetwork.firstlayer;

    while (currentlayer != NULL) {
      //printf("teste 1");
        // Libere a memória dos neurônios na camada atual
        Neuron *currentneuron = currentlayer->firstneuron;
        while (currentneuron != NULL) {
            //printf("teste 2");

            // Libere a memória dos pesos associados ao neurônio atual
            Weight *currentweight = currentneuron->firstweight;
            while (currentweight != NULL) {
                //printf("teste 3");

                Weight *nextweight = currentweight->nextweight;
                freeWeight(currentweight);
                currentweight = nextweight;
            }

            // Libere a memória do neurônio atual
            Neuron *nextneuron = currentneuron->nextneuron;
            freeNeuron(currentneuron);
            currentneuron = nextneuron;
        }

        // Libere a memória da camada atual
        Layer *nextlayer = currentlayer->nextlayer;
        freeLayer(currentlayer);
        currentlayer = nextlayer;
    }

    //free(neuralnetwork);
}

void freeVector(float *vector) {
    if (vector != NULL) {
        free(vector);
    }
}

void freeMatrix(float **matrix, int lines) {
    if (matrix != NULL) {
        for (int i = 0; i < lines; i++) {
            if (matrix[i] != NULL) {
                free(matrix[i]);
            }
        }
        free(matrix);
    }
}

//////////////////////////////////////////////////DEEPLEARNINGNEURALNETWORK//////////////////////////////////////////////////

float ActivationFunctionCalculaton(NeuralNetwork *neuralnetwork,float Z, int activationfunctiontype){

  //printf("activaiton function type: %d",activationfunctiontype);

  Layer *currentlayer;
  Neuron *currentneuron, *previousneuron;
  Weight *currentweight;

  float sum = 0;
  float softmaxsum=0;

switch (activationfunctiontype)
{
case 1:
  return Perceptron(Z);
case 2:
  return ReLU(Z);
case 3:
  return Sigmoid(Z); 
case 4:

      currentlayer = neuralnetwork->lastlayer;
      currentneuron = currentlayer -> firstneuron;
      previousneuron = currentlayer->previouslayer->firstneuron;

      for (int i = 0; i < currentlayer -> neurons; i++) {
        currentweight= currentneuron->firstweight;
        //printf("wieghts %d \nB %f - ",currentneuron->weights,currentneuron->bias);
        sum+= currentneuron->bias;
        for (int j = 0;  j< currentneuron -> weights; j++) {
          sum += currentweight->weight * previousneuron->activationfunctionvalue;
          //printf("W %f IN %f - ",currentweight->weight,previousneuron->activationfunctionvalue,Z);
          currentweight =currentweight->nextweight;
          previousneuron = previousneuron ->nextneuron;
        }
        softmaxsum += exp(sum);

        sum = 0;
        currentneuron = currentneuron -> nextneuron;
        previousneuron = currentlayer->previouslayer->firstneuron;
      }
      //printf("Exp Z %f SOFTMAXSUM += %f\n",exp(Z),softmaxsum);

      return SoftMax(Z,softmaxsum);

default:
  return 0;
}

}

float ActivationFunctionDerivativeCalculation(float a, int activationfunctiontype){
  
switch (activationfunctiontype){
  case 1:
    return PerceptronDerivative(a);
  case 2:
    return ReLUDerivative(a);
  case 3:
    return SigmoidDerivative(a); 

  default:
    return 0;
  }
}

float StochasticGradientDescentCalculation( float weight,float output ,float deltafunction,int regularization,float alpha,float lambda){

  switch (regularization){
    
    case 0:
      //printf("REGULATION 0 ");
      return -alpha * deltafunction * output;
    
    case 1:
      //printf("REGULATION 1 ");

      if(weight >0){
        return -alpha * (deltafunction * output + 1);

      }
      else if(weight==0){
        return -alpha * (deltafunction * output + 0);

      }
      else{
        return -alpha * (deltafunction * output -1);
        
      }
    case 2:
      //printf("REGULATION 2 ");

      return -alpha * (deltafunction * output + 2*lambda*weight);
      
    default:
      return 0;
  }
}

float RidgeRegressionCalculation(NeuralNetwork *neuralnetwork,float lambda){

  float sum=0;

  Layer *currentlayer;
  Neuron *currentneuron;
  Weight *currentweight;

  currentlayer = neuralnetwork->firstlayer; 
  while(currentlayer != NULL){
    currentneuron = currentlayer->firstneuron;

    while (currentneuron!=NULL){
      currentweight = currentneuron->firstweight;

      while (currentweight!=NULL){
        sum += currentweight->weight*currentweight->weight;
        currentweight = currentweight->nextweight;
      }
      currentneuron= currentneuron->nextneuron;
    }
      currentlayer = currentlayer->nextlayer;
  }
  return sum*lambda;
}

float LassoRegressionCalculation(NeuralNetwork *neuralnetwork,float lambda){
  
  float sum=0;

  Layer *currentlayer;
  Neuron *currentneuron;
  Weight *currentweight;

  currentlayer = neuralnetwork->firstlayer; 
  while(currentlayer != NULL){
    currentneuron = currentlayer->firstneuron;

    while (currentneuron!=NULL){
      currentweight = currentneuron->firstweight;

      while (currentweight!=NULL){

        if (currentweight->weight>=0){
          sum += currentweight->weight;
        }
        else{
          sum += -currentweight->weight;
        }

        currentweight = currentweight->nextweight;
      }
      currentneuron= currentneuron->nextneuron;
    }
      currentlayer = currentlayer->nextlayer;
  }
  return sum*lambda;
}

float LossFunctionCalculation(NeuralNetwork * neuralnetwork, float *labelvector, int regularization,float lambda){  
  
  float outputdata[neuralnetwork -> lastlayer->neurons];
  Neuron *currentneuron = neuralnetwork -> lastlayer -> firstneuron;
  
  //getting the neurons output value for loss function
  for (int i = 0; i < neuralnetwork->lastlayer->neurons; i++) {
    outputdata[i] = currentneuron -> activationfunctionvalue;
    currentneuron = currentneuron -> nextneuron;
  }

  switch (neuralnetwork->lossfunctiontype){
    case MINIMAL_MEAN_SQUARE:
      break;
    
    case CATEGORICAL_CROSS_ENTROPY:

      switch (regularization){
        
        case 0:
          return  CategoricalCrossEntropy(labelvector, outputdata, neuralnetwork->lastlayer->neurons) ;
        case 1:
          return  CategoricalCrossEntropy(labelvector, outputdata, neuralnetwork->lastlayer->neurons) + LassoRegressionCalculation(neuralnetwork,lambda);
        case 2:
          return  CategoricalCrossEntropy(labelvector, outputdata, neuralnetwork->lastlayer->neurons) + RidgeRegressionCalculation(neuralnetwork,lambda);
        default:
          break;
      }

      break;
    default:
      return 0;
  }

  return 0;
}

void FeedFoward(NeuralNetwork * neuralnetwork){

  Layer *currentlayer = neuralnetwork->firstlayer->nextlayer;
  Neuron *currentneuron;
  Weight *currentweight;
  
  //printf("FEEDFOWARD\n");

  for (int layer = 1; layer < neuralnetwork -> layers; layer++) {

    //printf("LAYER %d\n",layer);
            
    Neuron *previousneuron = currentlayer->previouslayer->firstneuron;
    currentneuron = currentlayer -> firstneuron;
    float Z = 0;
          
    //printf("neurons %d\n",currentlayer->neurons);

      for (int i = 0; i < currentlayer -> neurons; i++) {
        currentweight= currentneuron->firstweight;
        //printf("wieghts %d \nB %f - ",currentneuron->weights,currentneuron->bias);
        Z+= currentneuron->bias;
        for (int j = 0;  j< currentneuron -> weights; j++) {
          Z += currentweight->weight * previousneuron->activationfunctionvalue;
          //printf("W %f IN %f + ",currentweight->weight,previousneuron->activationfunctionvalue);
          currentweight =currentweight->nextweight;
          previousneuron = previousneuron ->nextneuron;
        }
        currentneuron -> activationfunctionvalue =  ActivationFunctionCalculaton(neuralnetwork,Z,currentlayer->activationfunctiontype);
        //printf("Z %f ATIVACAO %f\n",Z,currentneuron -> activationfunctionvalue);
        Z = 0;
        currentneuron = currentneuron -> nextneuron;
        previousneuron = currentlayer->previouslayer->firstneuron;
      }
    currentlayer = currentlayer -> nextlayer;
    }
}

void BackPropagation(NeuralNetwork *neuralnetwork, float *label, float alpha, int regularization, float lambda){
  //aux memory 
  int  weightscolumn=0, weightslines=0, deltafunctionsnumber=0;
  float **weights = NULL;
  float *deltafunctions = NULL;
  
  // variables to cycle through pointers
  Layer * currentlayer = neuralnetwork -> firstlayer;
  Neuron * currentneuron = currentlayer -> firstneuron;
  Weight * currentweight;

  currentlayer = neuralnetwork -> lastlayer;
  for (int layer = neuralnetwork -> layers ; layer > 1; layer--) {

    //verify the last layer
    if (layer == neuralnetwork -> layers) {

      //allocate memory for delta functions
      deltafunctionsnumber = currentlayer->neurons;
      deltafunctions = (float * ) malloc(deltafunctionsnumber * sizeof(float));
      currentneuron = currentlayer -> firstneuron;

      for (int i = 0; i < currentlayer->neurons; i++) {
        deltafunctions[i] =  currentneuron->activationfunctionvalue - label[i];
        currentneuron = currentneuron -> nextneuron;
      }


      //alocate memory for weights matrix
      weightslines = currentlayer ->previouslayer -> neurons;
      weightscolumn = currentlayer->neurons;

      weights = (float ** ) malloc(weightslines * sizeof(float * ));

      for (int i = 0; i < currentlayer -> previouslayer -> neurons; i++) {
        weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
      }

      currentneuron = currentlayer -> firstneuron;
      Neuron* previousneuron; 
          
      for (int i = 0; i < currentlayer->neurons; i++){  
        // currentneuron->bias += - deltafunctions[i] * alpha;
        currentneuron->bias += StochasticGradientDescentCalculation(currentneuron->bias, 1, deltafunctions[i], regularization, alpha, lambda); 
        previousneuron = currentlayer->previouslayer->firstneuron;
        currentweight = currentneuron->firstweight;

        for(int j=0;j<currentlayer->previouslayer->neurons ;j++){
          currentweight->weight += StochasticGradientDescentCalculation(
            currentweight->weight,
            previousneuron->activationfunctionvalue,
            deltafunctions[i],
            regularization,
            alpha,
            lambda
          );
          weights[j][i] = currentweight -> weight;
          previousneuron = previousneuron->nextneuron;
          currentweight = currentweight->nextweight;
        }
        currentneuron = currentneuron->nextneuron;
      }

    } 
    else{

      float * deltafunctionsaux = (float * ) malloc(deltafunctionsnumber * sizeof(float));
      memcpy(deltafunctionsaux, deltafunctions, deltafunctionsnumber * sizeof(float));
      deltafunctions = (float * ) realloc(deltafunctions, currentlayer->neurons * sizeof(int));
          
      float deltafunctionaccumulation;

      //calculatting delta
      currentneuron = currentlayer->firstneuron;
      for (int i = 0; i < weightslines; i++) {
        deltafunctionaccumulation = 0;
        for (int j = 0; j < weightscolumn; j++) {
          deltafunctionaccumulation += deltafunctionsaux[j] * weights[i][j];
        }
        deltafunctions[i] = deltafunctionaccumulation *  ActivationFunctionDerivativeCalculation(currentneuron->activationfunctionvalue,currentlayer->activationfunctiontype);
        currentneuron = currentneuron->nextneuron;
      }

      freeMatrix(weights,weightslines);

      weightscolumn = weightslines;
      weightslines = currentlayer -> previouslayer -> neurons;
      deltafunctionsnumber = weightslines;
          
      weights = (float ** ) malloc(weightslines * sizeof(float * ));          
      for (int i = 0; i < weightslines; i++) {
        weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
      }

      currentneuron = currentlayer->firstneuron;
      Neuron *previousneuron;
          
      for(int i = 0; i < currentlayer->neurons; i++){            
        // currentneuron->bias += - deltafunctions[i]*alpha;
        currentneuron->bias += StochasticGradientDescentCalculation(currentneuron->bias, 1, deltafunctions[i], regularization, alpha, lambda); 
        currentweight = currentneuron->firstweight;
        previousneuron = currentlayer->previouslayer->firstneuron;
        for (int j = 0; j < currentneuron->weights; j++){
          currentweight->weight +=  StochasticGradientDescentCalculation(
            currentweight->weight,
            previousneuron->activationfunctionvalue,
            deltafunctions[i],
            regularization,
            alpha,
            lambda
          );
          weights[j][i] = currentweight->weight;
          currentweight = currentweight->nextweight;
          previousneuron = previousneuron->nextneuron;
        }
        currentneuron = currentneuron -> nextneuron;
      }
      freeVector(deltafunctionsaux);
    }
    currentlayer = currentlayer -> previouslayer;
  }

  freeMatrix(weights,weightslines);
  freeVector(deltafunctions);
}

void NeuralNetworkTraining() {

  NeuralNetwork * neuralnetwork = getFederatedLearningInstance()->neuralnetwork;

  float trainingsample[neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons];
  float label[neuralnetwork->lastlayer->neurons];
  float Error = 0.0;

  FILE * file = NULL;
  char line[1024];
  Neuron * currentneuron = NULL;
  
  for (int TrainingCycle = 0; TrainingCycle < neuralnetwork->epoch; TrainingCycle++) {
    Error = 0.0;

    // file = fopen("/storage/dataset.csv", "r");
    file = fopen(DATASET_ADDR, "r");


    if (file == NULL) {
        perror("Error when opening CSV file.");
        return;
    }

    int count = 0;
    for( ; count < neuralnetwork->percentualtraining; count++){
      
      fgets(line, sizeof(line), file);

        char * token = strtok(line, ","); 

        for (int i = 0; i < 7; i++){
            token = strtok(NULL, ",");
            if (token != NULL) {
              trainingsample[i] = strtof(token, NULL);
            } else {
              printf("Error: Not enought data in the line.\n");
              break;
            }
        }


      //set the input data on inputdata vector
      currentneuron =  neuralnetwork->firstlayer->firstneuron;
      for (int i = 0; i < neuralnetwork->firstlayer->neurons; i++) {
        currentneuron -> activationfunctionvalue = trainingsample[i];
        currentneuron = currentneuron -> nextneuron;
      }

      //set the label on label vector
      for (int i = neuralnetwork->firstlayer->neurons; i < neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons; i++) {
        label[i - neuralnetwork->firstlayer->neurons] = trainingsample[i];
      }

      FeedFoward(neuralnetwork);
      Error += LossFunctionCalculation(neuralnetwork,label,neuralnetwork->regularization,neuralnetwork->lambda);
      BackPropagation(neuralnetwork,label,neuralnetwork->alpha,neuralnetwork->regularization,neuralnetwork->lambda);
    }

    // close file
    fclose(file);

    printf("Epoch %d Loss: %f\n", TrainingCycle+1, Error/count);
  }
}

//////////////////////////////////////////////////FEDERATEDLEARNING//////////////////////////////////////////////////

void replaceNeuralNetwork(FederatedLearning * newfederatedlearninginstance){
    FederatedLearning * federatedlearninginstance =  getFederatedLearningInstance();
    federatedlearninginstance->neuralnetwork = newfederatedlearninginstance->neuralnetwork;
    federatedlearninginstance->trainingscounter=0;
}

void mergeNeuralNetwork(FederatedLearning * newfederatedlearninginstance){

}

FederatedLearning *getFederatedLearningInstance() {
    
    static FederatedLearning instance;
    if (instance.neuralnetwork == NULL) {
        instance.neuralnetwork = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));
        instance.globalmodelstatus = 0;
        instance.trainingscounter = 0;
    }
    return &instance;
}

void readCSVFile() {

  float trainingsample[7];

  FILE * file = NULL;
  char line[1024];


    // file = fopen("/storage/dataset.csv", "r");
    file = fopen(DATASET_ADDR, "r");

    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {


        char * token = strtok(line, ","); 

        for (int i = 0; i < 7; i++){
            token = strtok(NULL, ",");
            if (token != NULL) {
              trainingsample[i] = strtof(token, NULL);
            } else {
            printf("Erro: Não há dados suficientes para preencher InputData na linha.\n");
            break;
            }
        }

      printf("InputData: [%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n", trainingsample[0], trainingsample[1], trainingsample[2], trainingsample[3], trainingsample[4], trainingsample[5], trainingsample[6]);
  }

}


void PerformanceMetrics(NeuralNetwork* neuralnetwork,int PercentualEvaluation,float Threshold){
  float trainingsample[neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons];
  float label[neuralnetwork->lastlayer->neurons];
  int total=0, truepositive=0, falsepositive=0, truenegative=0, falsenegative=0;
  Layer* currentlayer = neuralnetwork->firstlayer;
  Neuron* currentneuron = currentlayer->firstneuron;

  FILE* file = NULL;
  char line[1024];

  file = fopen(DATASET_ADDR, "r");

  if (file == NULL) {
    perror("Error reading the file!\n");
    return;
  }

  for(int count =0; count < PercentualEvaluation; count++){
    fgets(line, sizeof(line), file);

    // divide the line with ","
    char* token = strtok(line, ","); 

    // read line elements
    // printf("data %d", count);
    for (int i = 0; i < 7; i++){
      token = strtok(NULL, ",");
      if (token != NULL) {
        trainingsample[i] = strtof(token, NULL);
        // printf("\t%f", trainingsample[i]);
      } else {
        printf("Error: Not enough data in the line.\n");
        break;
      }
    }

    currentneuron =  neuralnetwork->firstlayer->firstneuron;
    for (int i = 0; i < neuralnetwork->firstlayer->neurons; i++) {
      currentneuron -> activationfunctionvalue = trainingsample[i];
      currentneuron = currentneuron -> nextneuron;
    }

    //set the label on label vector
    // printf("data labels %d:", count);
    for (int i = neuralnetwork->firstlayer->neurons; i < neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons; i++) {
      label[i - neuralnetwork->firstlayer->neurons] = trainingsample[i];
      // printf("\t%f", count, trainingsample[i]);
    }
    // printf("\n");

    FeedFoward(neuralnetwork);

    currentneuron = neuralnetwork->lastlayer->firstneuron;

    float max = -1.0;
    int arg_max = 0;
    int class_idx = 0;
    printf("activations: %d", count);
    for(int i=0; i < neuralnetwork->lastlayer->neurons; i++){
      if ((currentneuron->activationfunctionvalue) >= max) {
        max = currentneuron->activationfunctionvalue;
        arg_max = i;
      }

      if (label[i] == 1) {
        class_idx = i;
      }
      printf("\t%f", currentneuron->activationfunctionvalue);
      // if(label[i]==1){
      //   if(currentneuron->activationfunctionvalue > Threshold){
      //     truepositive++;
      //   }else{
      //     // falsepositive++;
      //     falsenegative++;
      //   }
      //   total++;
      // }else{
      //   if(currentneuron->activationfunctionvalue < Threshold){
      //     truenegative++;
      //   }else{
      //     // falsenegative++;
      //     falsepositive++;
      //   }
      //   total++;
      // }

      currentneuron = currentneuron->nextneuron;
    }
    printf("\n");
    // printf("argmax: %d\tclass_idx: %d\n", arg_max, class_idx);
    if (arg_max == class_idx) {
      truepositive++;
      // truenegative += 2;
    } else {
      falsepositive++;
      // falsenegative++;
      // truenegative++;
    }
    // total += 3;
    total++;
  }

  fclose(file);
  printf("total = %d, TP = %d, TN = %d, FP = %d, FN = %d\n",total,truepositive,truenegative,falsepositive,falsenegative);
  printf("Accuracy: %.2f\n",(float)(truenegative+truepositive)/(truenegative+truepositive+falsenegative+falsepositive));
  // printf("Precision: %.2f\n",(float)truepositive/(truepositive+falsepositive));
  // printf("Recall: %.2f\n",(float)truepositive/(truepositive+falsenegative));
  // printf("Specificity: %.2f\n",(float)truenegative/(truenegative+falsepositive));
  //printf("F1-Score: %.2f\n",F1Score(truepositive,falsepositive,falsenegative));
}
