#include "UtilPipelineInputManager.h"
#include <Windows.h>

UtilPipelineInputManager::UtilPipelineInputManager(void)
{
	rgb = NULL;
	out_rgb = NULL;
	coords = NULL;
	out_coords = NULL;
	uv = NULL;
	depth = NULL;
	out_depth = NULL;
	bodyLabels = NULL;
	labeltodatamap = NULL;
}


UtilPipelineInputManager::~UtilPipelineInputManager(void)
{
	if (rgb) delete [] rgb;
	if (out_rgb) delete [] out_rgb;
	if (coords) delete [] coords;
	if (out_coords) delete [] out_coords;
	if (uv) delete [] uv;
	if (depth) delete[] depth;
	if (out_depth) delete [] out_depth;
	if (bodyLabels) delete [] bodyLabels;
	if (labeltodatamap) delete [] labeltodatamap;
	//pipeline.Close();
}

bool UtilPipelineInputManager::init() {
	bodyLabels = new PXCGesture::GeoNode::Label[HandData::N_POINTS];
	labeltodatamap = new int[HandData::N_POINTS];
	bodyLabels[0] = PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT;
	bodyLabels[1] = PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT;
	bodyLabels[2] = PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT | PXCGesture::GeoNode::LABEL_FINGER_INDEX;
	bodyLabels[3] = PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT | PXCGesture::GeoNode::LABEL_FINGER_INDEX;
	bodyLabels[4] = PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT | PXCGesture::GeoNode::LABEL_FINGER_THUMB;
	bodyLabels[5] = PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT | PXCGesture::GeoNode::LABEL_FINGER_THUMB;
	bodyLabels[6] = PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT | PXCGesture::GeoNode::LABEL_FINGER_MIDDLE;
	bodyLabels[7] = PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT | PXCGesture::GeoNode::LABEL_FINGER_MIDDLE;
	bodyLabels[8] = PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT | PXCGesture::GeoNode::LABEL_FINGER_RING;
	bodyLabels[9] = PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT | PXCGesture::GeoNode::LABEL_FINGER_RING;
	bodyLabels[10] = PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT | PXCGesture::GeoNode::LABEL_FINGER_PINKY;
	bodyLabels[11] = PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT | PXCGesture::GeoNode::LABEL_FINGER_PINKY;
	bodyLabels[12] = PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT | PXCGesture::GeoNode::LABEL_HAND_FINGERTIP;
	bodyLabels[13] = PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT | PXCGesture::GeoNode::LABEL_HAND_FINGERTIP;
	labeltodatamap[0] = HandData::LEFT_HAND;
	labeltodatamap[1] = HandData::RIGHT_HAND;
	labeltodatamap[2] = HandData::LEFT_INDEX;
	labeltodatamap[3] = HandData::RIGHT_INDEX;
	labeltodatamap[4] = HandData::LEFT_THUMB;
	labeltodatamap[5] = HandData::RIGHT_THUMB;
	labeltodatamap[6] = HandData::LEFT_MIDDLE;
	labeltodatamap[7] = HandData::RIGHT_MIDDLE;
	labeltodatamap[8] = HandData::LEFT_RING;
	labeltodatamap[9] = HandData::RIGHT_RING;
	labeltodatamap[10] = HandData::LEFT_PINKY;
	labeltodatamap[11] = HandData::RIGHT_PINKY;
	labeltodatamap[12] = HandData::LEFT_TIP;
	labeltodatamap[13] = HandData::RIGHT_TIP;

	pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB32);
	pipeline.EnableImage(PXCImage::COLOR_FORMAT_VERTICES);
	pipeline.EnableGesture();

	if (!pipeline.Init()) return false;
	pipeline.QueryImageSize(PXCImage::IMAGE_TYPE_COLOR, wi, hi);
	rgb = new unsigned char[4*wi*hi];
	out_rgb = new float[3*wi*hi];
	pipeline.QueryImageSize(PXCImage::IMAGE_TYPE_DEPTH, w, h);
	depth = new short[w*h];
	out_depth = new float[w*h];

	out_coords = new float[3*w*h];
	coords = new short[3*w*h];
	uv = new float[2*w*h];

	return true;
}

void UtilPipelineInputManager::update() {
	PXCGesture* gestureData;
	PXCImage* img;
	PXCImage::ImageData data;
	int len = w*h;
	int leni = wi*hi;
	
	if (pipeline.AcquireFrame(false)) {
		img = pipeline.QueryImage(PXCImage::IMAGE_TYPE_DEPTH);
		if (img) {
			PXCImage::ImageInfo inf;
			img->QueryInfo(&inf);
			if (PXC_STATUS_NO_ERROR != img->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::COLOR_FORMAT_VERTICES, &data)) {
				OutputDebugString("Error getting depth data\n");
			}
			memcpy(coords, data.planes[0], 3*len*sizeof(short));
			memcpy(uv, data.planes[2], 2*len*sizeof(float));
			img->ReleaseAccess(&data);
			/*if (PXC_STATUS_NO_ERROR != img->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::COLOR_FORMAT_DEPTH, &data)) {
				OutputDebugString("Error getting depth data\n");
			}
			memcpy(depth, data.planes[0], len*sizeof(short));
			img->ReleaseAccess(&data);*/
		}
		img = pipeline.QueryImage(PXCImage::IMAGE_TYPE_COLOR);
		if (img) {
			if (PXC_STATUS_NO_ERROR != img->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::COLOR_FORMAT_RGB32, &data)) {
				OutputDebugString("Error getting color data\n");
			}
			memcpy(rgb, data.planes[0], 4*leni);
			img->ReleaseAccess(&data);
		}
		gestureData = pipeline.QueryGesture();
		if (gestureData) {
			PXCGesture::GeoNode geonode;
			for (int i = 0; i < HandData::N_POINTS; ++i) {
				if (PXC_STATUS_NO_ERROR == gestureData->QueryNodeData(0, bodyLabels[i], &geonode)) {
					handdata.setData(labeltodatamap[i], geonode.positionWorld.x, geonode.positionWorld.z, geonode.positionWorld.y);
					if (labeltodatamap[i] == HandData::LEFT_HAND || labeltodatamap[i] == HandData::RIGHT_HAND) {
						handdata.setOpenness(labeltodatamap[i], geonode.openness/100.);
					}
				} else {
					handdata.setNoData(labeltodatamap[i]);
				}
			}
		}
		pipeline.ReleaseFrame();
		// Normalize data
		for (int i = 0; i < len; ++i) {
			out_depth[i] = depth[i]/1000.;
			out_coords[3*i] = coords[3*i]/1000.;
			out_coords[3*i+1] = coords[3*i+1]/1000.;
			out_coords[3*i+2] = coords[3*i+2]/1000.;
			if (uv[i*2] <= 1. && uv[i*2] >= 0.) {
				int ind = (int)(uv[i*2]*wi) + (int)(uv[i*2+1]*hi)*wi;
				out_rgb[3*i]   = rgb[4*ind+2]/255.;
				out_rgb[3*i+1] = rgb[4*ind+1]/255.;
				out_rgb[3*i+2] = rgb[4*ind+0]/255.;
			} else {
				out_rgb[3*i]   = 64;
				out_rgb[3*i+1] = 0;
				out_rgb[3*i+2] = 0;
			}
		}
		/*for (int i = 0; i < h; ++i) {
			for (int j = 0; j < w; ++j) {
				// Dumb downsampling 
				out_rgb[3*(j+w*i)]   = ((int)rgb[4*(2*j+wi*2*i)+2] + rgb[4*(2*j+1+wi*2*i)+2] + rgb[4*(2*j+1+wi*(2*i+1))+2] + rgb[4*(2*j+wi*(2*i+1))+2])/1020.;
				out_rgb[3*(j+w*i)+1] = ((int)rgb[4*(2*j+wi*2*i)+1] + rgb[4*(2*j+1+wi*2*i)+1] + rgb[4*(2*j+1+wi*(2*i+1))+1] + rgb[4*(2*j+wi*(2*i+1))+1])/1020.;
				out_rgb[3*(j+w*i)+2] = ((int)rgb[4*(2*j+wi*2*i)+0] + rgb[4*(2*j+1+wi*2*i)+0] + rgb[4*(2*j+1+wi*(2*i+1))+0] + rgb[4*(2*j+wi*(2*i+1))+0])/1020.;
			}
		}*/
	}
}

float* UtilPipelineInputManager::getDepthFrame() {
	return out_depth;
}

void UtilPipelineInputManager::getRGBFrame(void* ptr) {
	memcpy(ptr, out_rgb, 3*4*w*h);
}

void UtilPipelineInputManager::getCoords(void* ptr) {
	memcpy(ptr, out_coords, 3*4*w*h);
}

void UtilPipelineInputManager::getDepthFrame(void* ptr) {
	memcpy(ptr, out_depth, 4*w*h);
}


float* UtilPipelineInputManager::getRGBFrame() {
	return out_rgb;
}

float* UtilPipelineInputManager::getCoords() {
	return out_coords;
}

HandData UtilPipelineInputManager::getSkelData() {
	return handdata;
}