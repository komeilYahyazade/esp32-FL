#include "../lib/federatedlearning.h"
#include "../lib/JSONConverter.h"
#include "../lib/trainingConfigs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

static pthread_mutex_t singletonMutex = PTHREAD_MUTEX_INITIALIZER;

//////////////////////////////////////////////////PRINT//////////////////////////////////////////////////

void PrintNeuralNeuralNetwork(NeuralNetwork * neuralnetwork) {

  printf("epoch: %d\n",neuralnetwork->epoch);
  printf("alpha: %.10f\n",neuralnetwork->alpha);
  printf("regularization: %d\n",neuralnetwork->regularization);
  printf("lambda: %.10f\n",neuralnetwork->lambda);
  printf("percentualtraining: %d\n",neuralnetwork->percentualtraining);
  printf("lossfunctiontype: %d\n",neuralnetwork->lossfunctiontype);
  printf("layers: %d\n",neuralnetwork->layers);

  int i = 0, j = 0, k = 0;

  Layer * currentlayer = neuralnetwork -> firstlayer;
  while (currentlayer != NULL) {

    i++;
    printf("activationfunctiontype: %d - layer %d - neurons: %d\n",currentlayer->activationfunctiontype,i,currentlayer -> neurons);
    Neuron * currentneuron = currentlayer -> firstneuron;

    while (currentneuron != NULL) {

      j++;
      printf("neurontype %s - neuron %d - weights: %d\n",currentneuron->neurontype , j,currentneuron -> weights);
      printf("bias: %.10f\n",currentneuron->bias);

      Weight * currentweight = currentneuron -> firstweight;

      while (currentweight != NULL) {

        k++;
        printf("weight: %d - value: %.10f ", k, currentweight -> weight);

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

//////////////////////////////////////////////////MEMORY//////////////////////////////////////////////////

Weight *CopyWeight(Weight *currentweight){
  
  Weight *newweight= (Weight *)malloc(sizeof(Weight));
  newweight->weight = currentweight->weight;
  return newweight;
}

Neuron *CopyNeuron(Neuron *neuronsource){

  Neuron *newnueron = (Neuron *)malloc(sizeof(Neuron));

  strcpy(newnueron->neurontype, neuronsource->neurontype);
  newnueron ->weights=neuronsource->weights;
  newnueron-> activationfunctionvalue=neuronsource->activationfunctionvalue;
  newnueron ->bias=neuronsource->bias;
  newnueron->firstweight = NULL;
  newnueron->lastweight = NULL;

  Weight *currentweight = neuronsource->firstweight;

  while (currentweight != NULL){

    Weight * newweight = CopyWeight(currentweight);

    if(newnueron->firstweight==NULL){
      newweight->previousweight=NULL;
      newweight->nextweight=NULL;
      newnueron->firstweight=newweight;
      newnueron->lastweight=newweight;
    }

    newweight->nextweight=NULL;
    newweight->previousweight= newnueron->lastweight;
    newnueron->lastweight->nextweight = newweight;
    newnueron->lastweight= newweight;

    currentweight = currentweight->nextweight;
  }
  return newnueron;
}

Layer *CopyLayer(Layer *layersource){

  Layer *newlayer = (Layer *)malloc(sizeof(Layer));
  
  newlayer->activationfunctiontype = layersource->activationfunctiontype;
  newlayer->neurons = layersource->neurons;
  newlayer->firstneuron = NULL;
  newlayer->lastneuron = newlayer->firstneuron;

  Neuron *currentneuron = layersource->firstneuron;

  while (currentneuron != NULL){
    Neuron *newneuron = CopyNeuron(currentneuron);

    if(newlayer->firstneuron==NULL){
      newneuron->previousneuron=NULL;
      newneuron->nextneuron=NULL;
      newlayer->firstneuron=newneuron;
      newlayer->lastneuron=newneuron;
    }

    newneuron->nextneuron=NULL;
    newneuron->previousneuron= newlayer->lastneuron;
    newlayer->lastneuron->nextneuron = newneuron;
    newlayer->lastneuron= newneuron;

    currentneuron = currentneuron->nextneuron;
  }

  return newlayer;
}

NeuralNetwork *CopyNeuralNetwork(NeuralNetwork* neuralnetworksource){

  NeuralNetwork * newneuralnetwork= (NeuralNetwork *)malloc(sizeof(NeuralNetwork));

  newneuralnetwork->epoch=neuralnetworksource->epoch;
  newneuralnetwork->alpha=neuralnetworksource->alpha;
  newneuralnetwork->regularization=neuralnetworksource->regularization;
  newneuralnetwork->lambda=neuralnetworksource->lambda;
  newneuralnetwork->percentualtraining=neuralnetworksource->percentualtraining;
  newneuralnetwork->lossfunctiontype=neuralnetworksource->lossfunctiontype;
  newneuralnetwork->layers=neuralnetworksource->layers;
  newneuralnetwork->firstlayer = NULL;
  newneuralnetwork->lastlayer = newneuralnetwork->firstlayer;

  Layer * currentLayer = neuralnetworksource->firstlayer;

  while (currentLayer != NULL){
    Layer *newlayer = CopyLayer(currentLayer);

    if(newneuralnetwork->firstlayer==NULL){
      newlayer->previouslayer=NULL;
      newlayer->nextlayer=NULL;
      newneuralnetwork->firstlayer=newlayer;
      newneuralnetwork->lastlayer=newlayer;
    }

    newlayer->nextlayer=NULL;
    newlayer->previouslayer= newneuralnetwork->lastlayer;
    newneuralnetwork->lastlayer->nextlayer = newlayer;
    newneuralnetwork->lastlayer= newlayer;

    currentLayer = currentLayer->nextlayer;
  }

  return newneuralnetwork;
}

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

void freeNeuralNetwork(NeuralNetwork *neuralnetwork) {
      //printf("teste 0");
    
    Layer *currentlayer = neuralnetwork->firstlayer;

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

    free(neuralnetwork);
}

//////////////////////////////////////////////////DEEPLEARNINGMATH//////////////////////////////////////////////////

float Perceptron(float Z){
    if(Z>0)
    {
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

float Sigmoid(float Z) {
  return 1.0 / (1.0 + exp(-Z));
}

float SoftMax( float Z,float softmaxsum ){
  return exp(Z)/softmaxsum;
}

float CategoricalCrossEntropy(float * label, float * output, int labelsize) {
  float sum = 0;
  for (int i = 0; i < labelsize; i++) {
    sum += label[i] * log(output[i]);
  }
  return -sum;
}

//////////////////////////////////////////////////DEEPLEARNINGNEURALNETWORK//////////////////////////////////////////////////

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
    //printf("label %f output %f \n",labelvector[i],outputdata[i]);
  }

  switch (neuralnetwork->lossfunctiontype){
  case MINIMAL_MEAN_SQUARE:
    break;
  
  case CATEGORICAL_CROSS_ENTROPY:

    switch (regularization){
      
      case 0:
        return  CategoricalCrossEntropy(labelvector, outputdata, neuralnetwork->lastlayer->neurons) ;
        break;

      case 1:
        return  CategoricalCrossEntropy(labelvector, outputdata, neuralnetwork->lastlayer->neurons) + LassoRegressionCalculation(neuralnetwork,lambda);
        break;

      case 2:
        return  CategoricalCrossEntropy(labelvector, outputdata, neuralnetwork->lastlayer->neurons) + RidgeRegressionCalculation(neuralnetwork,lambda);
        break;
      default:
        break;
    }
    break;
  default:
    return 0;
    break;

  }

  return 0;
  
}

float ActivationFunctionCalculaton(NeuralNetwork *neuralnetwork,float Z, int activationfunctiontype){

  Layer *currentlayer;
  Neuron *currentneuron, *previousneuron;
  Weight *currentweight;

  float sum = 0;
  float softmaxsum=0;

//printf("activaiton function type: %d",activationfunctiontype);

switch (activationfunctiontype){
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

void FeedFoward(NeuralNetwork * neuralnetwork){
  Layer *currentlayer = neuralnetwork->firstlayer->nextlayer;
  Neuron *currentneuron;
  Weight *currentweight;
  
  for (int layer = 1; layer < neuralnetwork -> layers; layer++) {
    Neuron *previousneuron = currentlayer->previouslayer->firstneuron;
    currentneuron = currentlayer -> firstneuron;
    float Z = 0;
          
    for (int i = 0; i < currentlayer -> neurons; i++) {
      currentweight = currentneuron->firstweight;
      Z += currentneuron->bias;
      for (int j = 0;  j < currentneuron->weights; j++) {
        Z += currentweight->weight * previousneuron->activationfunctionvalue;
        currentweight = currentweight->nextweight;
        previousneuron = previousneuron->nextneuron;
      }
      currentneuron -> activationfunctionvalue = ActivationFunctionCalculaton(neuralnetwork, Z, currentlayer->activationfunctiontype);
      Z = 0;
      currentneuron = currentneuron->nextneuron;
      previousneuron = currentlayer->previouslayer->firstneuron;
    }
    currentlayer = currentlayer->nextlayer;
  }
}

float WeightValue(int weightvalue){
  switch (weightvalue){
    case 0:
      return 0;
      break;
    case 1:
      return (float)(rand() % 100) / 100.0;
      break;
    case 2:
      return 0.5;
      break;
    default:
      break;
  }
  return 0;
}

void InitializeNeuralNetWork(NeuralNetwork * neuralnetwork,int layers,LayerConfig *layersconfig, int lossfunctiontype, int weightvalue) {

  LayerConfig *currrentlayerconfig = layersconfig->first;
  Layer *currrentlayer;

  //Inicialize a semente do gerador de números aleatórios
  srand((unsigned) time(NULL));

  neuralnetwork->layers = layers;
  neuralnetwork->lossfunctiontype = lossfunctiontype;
  neuralnetwork -> firstlayer = NULL;
  neuralnetwork -> lastlayer = NULL;

  currrentlayerconfig = layersconfig->first;
 
 for (int i = 0; i < neuralnetwork -> layers; i++) {

    Layer * newLayer = (Layer * ) malloc(sizeof(Layer));

    if (newLayer == NULL) {
      fprintf(stderr, "Alocation memory Failure\n");
      exit(1);
    }

    newLayer -> neurons = currrentlayerconfig->neurons;
    newLayer -> activationfunctiontype = currrentlayerconfig->activationfunctiontype;
    newLayer -> nextlayer = NULL;
    newLayer -> previouslayer = newLayer -> nextlayer;
    newLayer -> firstneuron = NULL;
    newLayer -> lastneuron = newLayer -> firstneuron;

    if (neuralnetwork -> lastlayer == NULL) {
      neuralnetwork -> firstlayer = newLayer;
    } else {
      neuralnetwork -> lastlayer -> nextlayer = newLayer;
      newLayer -> previouslayer = neuralnetwork -> lastlayer;
    }

    neuralnetwork -> lastlayer = newLayer;
    
    currrentlayer = newLayer;
    
    for (int j = 0; j < newLayer -> neurons; j++) {

      Neuron * newneuron = (Neuron * ) malloc(sizeof(Neuron));

      if (newneuron == NULL) {
        fprintf(stderr, "Alocation memory Failure\n");
        exit(1);
      }

      if (i == 0) {
        strcpy(newneuron->neurontype, "INPUT");
        newneuron -> weights = 0;
      }
      else if(i==neuralnetwork -> layers - 1){
        strcpy(newneuron->neurontype, "OUTPUT");
        newneuron -> weights = currrentlayer->previouslayer->neurons;
      } 
      else {
        strcpy(newneuron->neurontype, "HIDDEN");
        newneuron -> weights = currrentlayer->previouslayer->neurons;
      }

      //newneuron -> bias = (float)(rand() % 100) / 100.0; 
      newneuron -> bias = 0.0;

      newneuron -> nextneuron = NULL;
      newneuron -> previousneuron = newneuron -> nextneuron;
      newneuron -> firstweight = NULL;
      newneuron -> lastweight = newneuron -> firstweight;
      newneuron -> nextneuron = NULL;
      newneuron -> previousneuron = newneuron -> nextneuron;

      if (neuralnetwork -> lastlayer -> lastneuron == NULL) {

        neuralnetwork -> lastlayer -> firstneuron = newneuron;
      } else {

        neuralnetwork -> lastlayer -> lastneuron -> nextneuron = newneuron;
        newneuron -> previousneuron = neuralnetwork -> lastlayer -> lastneuron;
      }

      neuralnetwork -> lastlayer -> lastneuron = newneuron;

      for (int k = 0; k < newneuron -> weights; k++) {

        Weight * newweight = (Weight * ) malloc(sizeof(Weight));

        if (newweight == NULL) {
          fprintf(stderr, "Alocation memory Failure\n");
          exit(1);
        }
        
        newweight -> weight = WeightValue(weightvalue); 
        newweight -> nextweight = NULL;
        newweight -> previousweight = newweight -> nextweight;

        if (newneuron -> lastweight == NULL) {
          neuralnetwork -> lastlayer -> lastneuron -> firstweight = newweight;
        } else {
          neuralnetwork -> lastlayer -> lastneuron -> lastweight -> nextweight = newweight;
          newweight -> previousweight = neuralnetwork -> lastlayer -> lastneuron -> lastweight;
        }
        newneuron -> lastweight = newweight;
      }
    }

    currrentlayerconfig = currrentlayerconfig->next;
  }
}

//////////////////////////////////////////////////FEDERATEDLEARNING//////////////////////////////////////////////////

FederatedLearning *getFederatedLearningInstance() {

    static FederatedLearning instance;
    pthread_mutex_lock(&singletonMutex);
    if (instance.neuralnetwork == NULL) {
        instance.neuralnetwork = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));
        instance.globalmodelstatus = 0;
        instance.trainingscounter = 0;

        instance.nodecontrol = (NodeControl *)malloc(sizeof(NodeControl));
        instance.nodecontrol->firstclientnode=NULL;
        instance.nodecontrol->lastclientnode=instance.nodecontrol->firstclientnode;
        instance.nodecontrol->currentinteraction=0;
        instance.nodecontrol->neuralnetwork = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));
    }
    pthread_mutex_unlock(&singletonMutex);  
    return &instance;
}

void setFederatedLearningGlobalModel() {

  FederatedLearning *federatedLearningInstance = getFederatedLearningInstance();
  LayerConfig *layerconfig0 = malloc(sizeof(LayerConfig));
  LayerConfig *layerconfig1 = malloc(sizeof(LayerConfig));
  LayerConfig *layerconfig2 = malloc(sizeof(LayerConfig));
  LayerConfig *layerconfig3 = malloc(sizeof(LayerConfig));

  layerconfig0->first =layerconfig0;
  layerconfig0->neurons= 4;
  layerconfig0->activationfunctiontype = 0;
  layerconfig0->next = layerconfig1;

  layerconfig1->neurons= 3;
  layerconfig1->activationfunctiontype = RELU;
  layerconfig1->next = layerconfig2;

  layerconfig2->neurons= 3;
  layerconfig2->activationfunctiontype = RELU;
  layerconfig2->next = layerconfig3;

  layerconfig3->neurons= 3;
  layerconfig3->activationfunctiontype = SOFTMAX;
  layerconfig3->next = NULL;

  InitializeNeuralNetWork(federatedLearningInstance->neuralnetwork, 4 ,layerconfig0, CATEGORICAL_CROSS_ENTROPY,WEIGHT_VALUE_RANDOM);
  
  federatedLearningInstance->neuralnetwork->alpha = ALPHA;
  federatedLearningInstance->neuralnetwork->epoch = EPOCH;
  federatedLearningInstance->neuralnetwork->regularization = REGULARIZATION;
  federatedLearningInstance->neuralnetwork->lambda = LAMBDA;
  federatedLearningInstance->neuralnetwork->percentualtraining = PERCENTUALTRAINING;

  federatedLearningInstance->nodecontrol->interactioncycle = INTERCATIONCYCLE;
  federatedLearningInstance->nodecontrol->clientnodes = CLIENTNODES;
  
  InitializeNeuralNetWork(federatedLearningInstance->nodecontrol->neuralnetwork, 4 ,layerconfig0, CATEGORICAL_CROSS_ENTROPY,WEIGHT_VALUE_ZERO);
  federatedLearningInstance->nodecontrol->neuralnetwork->alpha = ALPHA;
  federatedLearningInstance->nodecontrol->neuralnetwork->epoch = EPOCH;
  federatedLearningInstance->nodecontrol->neuralnetwork->regularization = REGULARIZATION;
  federatedLearningInstance->nodecontrol->neuralnetwork->lambda = LAMBDA;
  federatedLearningInstance->nodecontrol->neuralnetwork->percentualtraining = PERCENTUALTRAINING;

  federatedLearningInstance->globalmodelstatus=1;

  printf("Neural Network Compiled!\n");

}

void FederatedAveraging(NeuralNetwork *globalneuralnetwork, int currenttrainingcounter){

  Layer * currentgloballayer = globalneuralnetwork -> firstlayer;
  while (currentgloballayer != NULL) {
    Neuron * currentglobalneuron = currentgloballayer -> firstneuron;
    while (currentglobalneuron != NULL) {
      //printf("bias:%.5f / %d = %.5f \n",currentglobalneuron->bias,currenttrainingcounter,currentglobalneuron->bias / currenttrainingcounter);
      currentglobalneuron->bias = currentglobalneuron->bias / currenttrainingcounter;
      Weight * currentglobalweight = currentglobalneuron -> firstweight;
      while (currentglobalweight != NULL) {
        //printf("weight:%.5f / %d = %.5f \n",currentglobalweight->weight,currenttrainingcounter,currentglobalweight->weight/currenttrainingcounter);
        currentglobalweight->weight = currentglobalweight->weight/currenttrainingcounter;
        currentglobalweight = currentglobalweight -> nextweight;
      }
      currentglobalneuron = currentglobalneuron -> nextneuron;
    }
    currentgloballayer = currentgloballayer -> nextlayer;
  }
}

void FederatedAveragingSum(NeuralNetwork *globalneuralnetwork, NeuralNetwork *clientneuralnetwork, int clienttrainingcounter){

  Layer * currentgloballayer = globalneuralnetwork -> firstlayer;
  Layer * currentclientlayer = clientneuralnetwork -> firstlayer;

  while (currentgloballayer != NULL) {

    Neuron * currentglobalneuron = currentgloballayer -> firstneuron;
    Neuron * currentclientneuron = currentclientlayer -> firstneuron;

    while (currentglobalneuron != NULL) {
      //printf("bias:%.5f * %d = %.5f\n",currentclientneuron->bias,clienttrainingcounter,currentclientneuron->bias*clienttrainingcounter);
      currentglobalneuron->bias += currentclientneuron->bias*clienttrainingcounter;
      Weight * currentglobalweight = currentglobalneuron -> firstweight;
      Weight * currentclientweight = currentclientneuron -> firstweight;

      while (currentglobalweight != NULL) {

        //printf("weight:%.5f * %d = %.5f\n",currentclientweight->weight,clienttrainingcounter,currentclientweight->weight*clienttrainingcounter);
        currentglobalweight->weight += currentclientweight->weight*clienttrainingcounter;
        currentglobalweight = currentglobalweight -> nextweight;
        currentclientweight = currentclientweight-> nextweight;
      }
      currentglobalneuron = currentglobalneuron -> nextneuron;
      currentclientneuron = currentclientneuron -> nextneuron;
    }
    currentgloballayer = currentgloballayer -> nextlayer;
    currentclientlayer = currentclientlayer -> nextlayer;
  }
}

void SetNeuralNetworkZeroWeightValue(NeuralNetwork *globalneuralnetwork){
  Layer * currentgloballayer = globalneuralnetwork -> firstlayer;
  while (currentgloballayer != NULL) {
    Neuron * currentglobalneuron = currentgloballayer -> firstneuron;
    currentglobalneuron->bias = 0;
    while (currentglobalneuron != NULL) {
      Weight * currentglobalweight = currentglobalneuron -> firstweight;
      while (currentglobalweight != NULL) {
        currentglobalweight->weight = 0;
        currentglobalweight = currentglobalweight -> nextweight;
      }
      currentglobalneuron = currentglobalneuron -> nextneuron;
    }
    currentgloballayer = currentgloballayer -> nextlayer;
  }
}

void AggregationModel(FederatedLearning* clientmodel){
  // printf("\n\ntesting received client model: \n");
  // PerformanceMetrics(clientmodel->neuralnetwork, 30, 0.5);
  // PrintNeuralNeuralNetwork(clientmodel->neuralnetwork);
  // printf("client model testing done!\n\n\n");

  FederatedLearning* federatedlearninginstance = getFederatedLearningInstance();

  federatedlearninginstance->globalmodelstatus=0;

  FederatedAveragingSum(federatedlearninginstance->nodecontrol->neuralnetwork,
                  clientmodel->neuralnetwork,
                  clientmodel->trainingscounter);
                  
  federatedlearninginstance->trainingscounter += clientmodel->trainingscounter;

  ClientNode *currentclientnode = federatedlearninginstance->nodecontrol->firstclientnode;
  
  // check if we have received all clients' updates, otherwise return and set the global status 1
  while (currentclientnode!=NULL){
    if(federatedlearninginstance->nodecontrol->currentinteraction==currentclientnode->interaction){
      federatedlearninginstance->globalmodelstatus=1;
      return;
    }
    currentclientnode = currentclientnode->nextclientnode;
  }
  
  //calculate the final weight value and set 0 the global training counter
  FederatedAveraging(federatedlearninginstance->nodecontrol->neuralnetwork,federatedlearninginstance->trainingscounter);
  federatedlearninginstance->trainingscounter = 0;
  
  //free the global neural network and copy the neuralnetwork to the global one
  freeNeuralNetwork(federatedlearninginstance->neuralnetwork);
  federatedlearninginstance->neuralnetwork = CopyNeuralNetwork(federatedlearninginstance->nodecontrol->neuralnetwork);
  // set 0 the neural network
  SetNeuralNetworkZeroWeightValue(federatedlearninginstance->nodecontrol->neuralnetwork);

  printf("test aggerated model\n");
  // PrintNeuralNeuralNetwork(federatedlearninginstance->neuralnetwork);
  PerformanceMetrics(federatedlearninginstance->neuralnetwork,30,0.5);
  federatedlearninginstance->nodecontrol->currentinteraction++;
  federatedlearninginstance->globalmodelstatus=1;
  printf("aggregation done!\n");
}

float Accuracy(int truepositive,int truenegative,int falsepositive,int falsenegative){
  return (float)(truenegative+truepositive)/(truenegative+truepositive+falsenegative+falsepositive);
}

float Precision(int truepositive,int falsepositive){
  return (float)truepositive/(truepositive+falsepositive);
}

float Recall(int truepositive,int falsenegative){
  return (float)truepositive/(truepositive+falsenegative);
}

float Specificity(int truenegative,int falsepositive){
  return (float)truenegative/(truenegative+falsepositive);
}

float F1Score(int truepositive,int falsepositive,int falsenegative){
  return 2*(float)(Precision(truepositive,falsepositive) * 
                  Recall(truepositive,falsenegative))/(
                  Precision(truepositive,falsepositive) + 
                  Recall(truepositive,falsenegative));
}

void PerformanceMetrics(NeuralNetwork* neuralnetwork,int PercentualEvaluation,float Threshold){
  float trainingsample[neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons];
  float label[neuralnetwork->lastlayer->neurons];
  int total=0, truepositive=0, falsepositive=0, truenegative=0, falsenegative=0;
  Layer* currentlayer = neuralnetwork->firstlayer;
  Neuron* currentneuron = currentlayer->firstneuron;

  FILE* file = NULL;
  char line[1024];

  file = fopen("datasetevaluation.csv", "r");

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
