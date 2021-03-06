/*
	MCMC chain object
	Author: Peter Hague
	Created: 20/07/14
*/
#include "include/chain.hpp"
#include "include/options.hpp"
#include "include/pick.hpp"
#include <iostream>

chain::chain() {
}

void chain::init(options *o) {
	double p;
	int i;

	ransource.initialise(1);

	width = o->getdoubleval("nparams");
	for (i=0;i<width;i++) {
		//Get parameters from options object
		rangeStart.push_back(o->getdoubleval("lowerlimit", i));
		dRange.push_back((o->getdoubleval("upperlimit", i)-rangeStart.back())/lintmax);
		stepSize.push_back((uint32_t)(o->getdoubleval("stepsize", i)/dRange.back()));
		startMean.push_back(o->getdoubleval("startmean", i));
		startDev.push_back(o->getdoubleval("startdev", i));
		if ((o->getstringval("limtype", i)).compare("flat")==0) limits.push_back(lim_flat);
		if ((o->getstringval("limtype", i)).compare("periodic")==0) limits.push_back(lim_periodic);
		if ((o->getstringval("limtype", i)).compare("gaussian")==0) limits.push_back(lim_gaussian);


		//Pick some starting values
		p = startMean.back()+startDev.back()*ransource.getNorm();
		data.push_back((uint32_t)((p-rangeStart.back())/dRange.back()));
		buffer.push_back(data[i]);

		//std::cout << "Parameter " << o->getstringval("paramname", i) << ": " << rangeStart[i] << "+n*" << dRange[i] << std::endl;
	}
}

void chain::setStep(uint16_t n, double value) {
	stepSize[n] = (uint32_t)(value/dRange[n]);
}

double chain::getStep(uint16_t n) {
	return (double)stepSize[n]*dRange[n];
}

void chain::getModel(uint32_t index, double *output) {
	index*=width;
	for(int i=0;i<width;i++) {
		output[i] = rangeStart[i]+(double)data[index+i]*dRange[i];
	}
}

void chain::getRawModel(uint32_t index, uint32_t *output) {
	index*=width;
	for(int i=0;i<width;i++) {
		output[i] = data[index+i];
	}
}

void chain::last(double *output) {
	int pointer = data.size()-width;
	for(int i=0;i<width;i++) {
  		output[i] = rangeStart[i]+(double)data[pointer+i]*dRange[i];
  	}
}

void chain::rawlast(uint32_t *output) {
	int pointer = data.size()-width;
	for(int i=0;i<width;i++) {
  		output[i] = data[pointer+i];
  	}
}

void chain::step() {
	int64_t p, np, dp;
	uint32_t top[width];
	rawlast(top);
	for(int i=0;i<width;i++) {
		p = (int64_t)top[i];
		dp = (int64_t)(ransource.getNorm() * (double)stepSize[i]);
		np = p + dp;
		if (limits[i]==lim_flat) {
			if (np<0) np=-np;
			if (np>lintmax) np=2L*(int64_t)lintmax-np;
		}
		if (limits[i]==lim_periodic) {
			if (np<0) np+=(int64_t)lintmax;
			if (np>lintmax) np-=(int64_t)lintmax;
		}
		buffer[i] = (uint32_t)np;
	}
}

void chain::makestep(double *newbuffer) {
	for(int i=0;i<width;i++) {
		buffer[i] = newbuffer[i]/dRange[i]-rangeStart[i];
	}
}

void chain::current(double *output) {
	for(int i=0;i<width;i++) {
  		output[i] = rangeStart[i]+(double)buffer[i]*dRange[i];
  	}
}

void chain::repeat() {
	uint32_t top[width];
	rawlast(top);
	for(int i=0;i<width;i++) {
		data.push_back(top[i]);
	}
}

void chain::push() {
	for(int i=0;i<width;i++) {
		data.push_back(buffer[i]);
	}
}

void chain::pop() {
	for(int i=0;i<width;i++) {
		data.pop_back();
	}
}

int chain::size() {
	return data.size()/(double)width;
}
