#include "Mesh.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <exception>
#include <set>

using namespace std;

Mesh::Mesh(void)
{
	vertices = NULL;
	colors = NULL;
	texture = NULL;
	use_color = false;

	for (int i = 0; i < 3; ++i) {
		translation[i] = 0;
		scale[i] = 1;
		rotation[i] = 0;
	}
}

Mesh::Mesh(const char* filename, Renderer* r, bool reverse) {
	render = r;
	ReadOff(filename, reverse);
	for (int i = 0; i < 3; ++i) {
		translation[i] = 0;
		scale[i] = 1;
		rotation[i] = 0;
	}
}

bool isblank(string s) {
	for (unsigned int i = 0; i < s.size(); ++i) {
		if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != '#') return false;
		else if (s[i] == '#') return true;
	}
	return true;
}

inline void getnextline(ifstream& in, string& line) {
	do {
		getline(in, line);
	} while (isblank(line));
	
}

bool Mesh::ReadOff(const char* filename, bool reverse) {
	ifstream in(filename);
	if (in.fail()) throw OFFFileFormatError("Unable to open file");
	// OFF File
	string line;
	getnextline(in, line);
	if (line != "OFF") throw OFFFileFormatError("Invalid OFF Header");
	getnextline(in, line);
	if (sscanf_s(line.c_str(), "%d %d %d", &nvert, &nface, &nedge) != 3) {
		throw OFFFileFormatError("Invalid OFF line counts"); 
	}
	vertices = new float[nvert*3];
	colors = new float[nvert*4];
	texture = new float[nvert*2];
	normals = new float[nvert*3];
	indices = new unsigned int[nface*3];

	for (int i = 0; i < nvert; ++i) {
		getnextline(in, line);
		int nread = sscanf_s(line.c_str(), "%f %f %f %f %f %f %f %f", &vertices[3*i], &vertices[3*i+1], &vertices[3*i+2], &colors[4*i], &colors[4*i+1], &colors[4*i+2], &colors[4*i+3]);
		if (nread < 3 || nread == 4) {
			throw OFFFileFormatError("Incomplete vertex specification");
		} else if (nread == 5) {
			texture[2*i] = colors[4*i]; texture[2*i+1] = colors[4*i+1];
			colors[4*i] = 0; colors[4*i+1] = 0; colors[4*i+2] = 0; colors[4*i+3] = 0;
		} else {
			use_color = (nread != 3);
			if (nread == 6) colors[4*i+3] = 1;
		}
	}

	for (int i = 0; i < nface; ++i) {
		getnextline(in, line);
		int n;
		// Only handle triangles
		if (!sscanf_s(line.c_str(), "%d", &n)) {
			throw OFFFileFormatError("Invalid face specification");
		}
		if (n != 3) {
			throw OFFFileFormatError("Invalid face size - must be triangle");
		} else {
			int a,b,c;
			if (sscanf_s(line.c_str(), "%d %d %d %d", &n,&a,&b,&c) != 4) {
				throw OFFFileFormatError("Incomplete face specification");
			}
			if (reverse) {
				indices[3*i] = b; indices[3*i+1] = a; indices[3*i+2] = c;
			} else {
				indices[3*i] = a; indices[3*i+1] = b; indices[3*i+2] = c;
			}
		}
	}
	calcNormals();
	return true;
}

void Mesh::calcNormals() {
	// Construct adjacency list
	std::set<int>* adj = new std::set<int>[nvert];
	for (int i = 0; i < nface*3; i += 3) {
		adj[indices[i]].insert(indices[i+1]);
		adj[indices[i]].insert(indices[i+2]);
		adj[indices[i+1]].insert(indices[i]);
		adj[indices[i+1]].insert(indices[i+2]);
		adj[indices[i+2]].insert(indices[i]);
		adj[indices[i+2]].insert(indices[i+1]);
	}

	// Add vectors to each face for normals
	std::set<int>::iterator it;
	for (int i = 0; i < nvert; ++i) {
		normals[3*i] = 0.f; normals[3*i+1] = 0.f; normals[3*i+2] = 0.f;
		for (it = adj[i].begin(); it != adj[i].end(); it++) {
			for (int j = 0; j < 3; ++j) normals[3*i+j] += (vertices[3*i+j] - vertices[3*(*it)+j]);
		}
	}
	// Renormalize
	for (int i = 0; i < nvert; ++i) {
		float tot = sqrt(normals[3*i]*normals[3*i] + normals[3*i+1]*normals[3*i+1] + normals[3*i+2]*normals[3*i+2]);
		normals[3*i] /= tot; normals[3*i+1] /= tot; normals[3*i+2] /= tot;
	}
}

Mesh::~Mesh(void)
{
	if (normals) delete [] normals;
	if (vertices) delete [] vertices;
	if (colors) delete [] colors;
	if (texture) delete [] texture;
	if (indices) delete [] indices;
}
