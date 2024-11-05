#include <QVector>
#pragma once


const double BIAS_INPUT = 1;
const double MAX_EPOCHS = 100;
const double learning_rate = 0.01;
double integration_function(const QVector<double>& input, const QVector<double>& weights);
int activation_function(const double input);
void training_function(QVector<QVector<double>>& input, QVector<double>& weights);
int classify(const QVector<double>& input, const QVector<double>& weights);
void randomize_weights(QVector<double>& weights);

