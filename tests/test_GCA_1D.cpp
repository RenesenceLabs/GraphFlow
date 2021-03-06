// Framework: GraphFlow
// Author: Machine Learning Group of UChicago
// Main Contributor: Hy Truong Son
// Institution: Department of Computer Science, The University of Chicago
// Copyright 2017 (c) UChicago. All rights reserved.

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "../GraphFlow/GCA_1D.h"

using namespace std;

const int nLevels = 5;
const int max_nVertices = 50;
const int nFeatures = 4;
const int nHiddens = 10;
const int nDepth = 3;
const int max_Radius = 1;
const double momentum_param = 0.9;

const double learning_rate = 0.1;
const int nEpochs = 100;

const int nMolecules = 4;

string model_fn = "GCA_1D-model.dat";

GCA_1D train_network(nLevels, max_nVertices, nFeatures, nHiddens, nDepth, max_Radius, momentum_param);
GCA_1D test_network(nLevels, max_nVertices, nFeatures, nHiddens, nDepth, max_Radius, momentum_param);

struct Molecule {
	DenseGraph *graph;
	vector< pair<int, int> > edge;
	vector< string > label;

	void build() {
		for (int i = 0; i < graph -> nVertices; ++i) {
			graph -> adj[i][i] = 1;
		}

		for (int i = 0; i < edge.size(); ++i) {
			int u = edge[i].first;
			int v = edge[i].second;
			graph -> adj[u][v] = 1;
			graph -> adj[v][u] = 1;
		}

		for (int v = 0; v < graph -> nVertices; ++v) {
			if (label[v] == "C") {
				graph -> feature[v][0] = 1.0;
			}
			if (label[v] == "H") {
				graph -> feature[v][1] = 1.0;
			}
			if (label[v] == "N") {
				graph -> feature[v][2] = 1.0;
			}
			if (label[v] == "O") {
				graph -> feature[v][3] = 1.0;
			}
		}
	}
};
Molecule **molecule;

void init(Molecule *mol, string name) {
	if (name == "CH4") {
		mol -> graph = new DenseGraph(5, nFeatures);

		mol -> edge.clear();
		mol -> edge.push_back(make_pair(0, 1));
		mol -> edge.push_back(make_pair(0, 2));
		mol -> edge.push_back(make_pair(0, 3));
		mol -> edge.push_back(make_pair(0, 4));

		mol -> label.clear();
		mol -> label.push_back("C");
		mol -> label.push_back("H");
		mol -> label.push_back("H");
		mol -> label.push_back("H");
		mol -> label.push_back("H");

		mol -> build();
	}

	if (name == "NH3") {
		mol -> graph = new DenseGraph(4, nFeatures);

		mol -> edge.clear();
		mol -> edge.push_back(make_pair(0, 1));
		mol -> edge.push_back(make_pair(0, 2));
		mol -> edge.push_back(make_pair(0, 3));

		mol -> label.clear();
		mol -> label.push_back("N");
		mol -> label.push_back("H");
		mol -> label.push_back("H");
		mol -> label.push_back("H");

		mol -> build();
	}

	if (name == "H2O") {
		mol -> graph = new DenseGraph(3, nFeatures);

		mol -> edge.clear();
		mol -> edge.push_back(make_pair(0, 1));
		mol -> edge.push_back(make_pair(0, 2));

		mol -> label.clear();
		mol -> label.push_back("O");
		mol -> label.push_back("H");
		mol -> label.push_back("H");

		mol -> build();
	}

	if (name == "C2H4") {
		mol -> graph = new DenseGraph(6, nFeatures);

		mol -> edge.clear();
		mol -> edge.push_back(make_pair(0, 1));
		mol -> edge.push_back(make_pair(0, 2));
		mol -> edge.push_back(make_pair(0, 3));
		mol -> edge.push_back(make_pair(3, 4));
		mol -> edge.push_back(make_pair(3, 5));

		mol -> label.clear();
		mol -> label.push_back("C");
		mol -> label.push_back("H");
		mol -> label.push_back("H");
		mol -> label.push_back("C");
		mol -> label.push_back("H");
		mol -> label.push_back("H");

		mol -> build();
	}
}

int main(int argc, char **argv) {
	molecule = new Molecule* [nMolecules];
	for (int i = 0; i < nMolecules; ++i) {
		molecule[i] = new Molecule();
	}

	init(molecule[0], "CH4");
	init(molecule[1], "NH3");
	init(molecule[2], "H2O");
	init(molecule[3], "C2H4");

	cout << "--- Learning ------------------------------" << endl;

	DenseGraph **graphs = new DenseGraph* [nMolecules];
	for (int i = 0; i < nMolecules; ++i) {
		graphs[i] = molecule[i] -> graph;
	}

	for (int j = 0; j < nEpochs; ++j) {
		pair<double, double> info = train_network.BatchLearn(nMolecules, graphs, learning_rate);
		cout << "Epoch " << (j + 1) << ":" << endl;
		cout << "    Before training: " << info.first << endl;
		cout << "    After training: " << info.second << endl;
	}

	// Save model to file
	train_network.save_model(model_fn);

	cout << endl << "--- Predicting ----------------------------" << endl;

	// Load model from file
	test_network.load_model(model_fn);

	double **predict = new double* [max_nVertices];
	for (int i = 0; i < max_nVertices; ++i) {
		predict[i] = new double [max_nVertices];
	}

	for (int i = 0; i < nMolecules; ++i) {
		cout << endl << "Molecule " << (i + 1) << ": ";

		test_network.Predict(molecule[i] -> graph, predict);

		cout << "Expected adjacency:" << endl;
		for (int x = 0; x < molecule[i] -> graph -> nVertices; ++x) {
			for (int y = 0; y < molecule[i] -> graph -> nVertices; ++y) {
				cout << molecule[i] -> graph -> adj[x][y] << " ";
			}
			cout << endl;
		}

		cout << endl;
		cout << "Predicted adjacency:" << endl;
		for (int x = 0; x < molecule[i] -> graph -> nVertices; ++x) {
			for (int y = 0; y < molecule[i] -> graph -> nVertices; ++y) {
				cout << predict[x][y] << " ";
			}
			cout << endl;
		}
	}
}