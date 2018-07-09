#pragma once
class EModularRenderSettings
{
public:
	EModularRenderSettings();
	~EModularRenderSettings();
	
	bool useBasicVolumetricLighting = true;
	float basicVolumetricLightingResolutionMultiplyer = 0.5f;
	float basicVolumetricLightingMaxBrightness = 0.7f;
	int basicVolumetricLightingSteps = 120;

	bool useSSR = true;
};

