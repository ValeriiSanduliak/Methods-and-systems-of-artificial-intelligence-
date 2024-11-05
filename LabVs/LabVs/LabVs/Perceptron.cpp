#include "Perceptron.h"
#include <vector>
#include <numeric>
#include <random>
#include <iostream>
using std::vector;


double integration_function(const QVector<double>& input, const QVector<double>& weights)
{
    return std::inner_product(input.begin(), input.end() - 1, weights.begin(), 0.0);
}

int activation_function(const double input)
{
	if (input >= 0) {
		return 1;
	}
	else {
		return 0;
	}
}

void training_function(QVector<QVector<double>>& input, QVector<double>& weights)
{
	int incorrect = 0;

	for (int i = 0; i < MAX_EPOCHS; i++) {
		incorrect = 0;
		std::random_device seed;
		shuffle(input.begin(), input.end(), seed);

		for (int j = 0; j < input.size(); j++) {
			double dot = integration_function(input[j], weights);
			dot += BIAS_INPUT * weights.back();
			int calculatedOutput = activation_function(dot);
			double error = input[j].back() - calculatedOutput;

			if (error != 0) {
				incorrect++;
				//Perform training
				for (int k = 0; k < weights.size() - 1; k++) {
					weights[k] += learning_rate * input[j][k] * error;
				}
				weights.back() += learning_rate * BIAS_INPUT * error;

				// Виведення оновленого стану ваг
				std::cout << "Updated weights: ";
				for (const auto& weight : weights) {
					std::cout << weight << " ";
				}
				std::cout << "\n";
			}
		}

		std::cout << "Incorrect : " << incorrect << "\n";
		if (incorrect == 0) {
			break;
		}
	}
}

int classify(const QVector<double>& input, const QVector<double>& weights)
{
	double dot = integration_function(input, weights);
	dot += BIAS_INPUT * weights.back();
	return activation_function(dot);
}

void randomize_weights(QVector<double>& weights)
{
	std::random_device seed;
	std::default_random_engine generator(seed());
	std::uniform_real_distribution<double> distributionDouble(-.5, .5);
	for (int i = 0; i < weights.size(); i++) {
		weights[i] = distributionDouble(generator);
	}
}
