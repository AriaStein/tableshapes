#pragma once
#include <juce_core/juce_core.h>
enum PARAM {
	INGAIN,
	GAIN,
	INDEX,
	TOTAL_NUMBER_PARAMETERS
};
static const std::array<juce::Identifier, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_IDS{
	"INGAIN",
	"GAIN",
	"INDEX",
};
static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_NAMES{
	"INGAIN",
	"GAIN",
	"INDEX",
};
static const std::array<juce::NormalisableRange<float>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_RANGES {
	juce::NormalisableRange<float>(0.0f, 100.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 100.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 255.0f, 1.0f, 1.0f),
};
static const std::array<float, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_DEFAULTS {
	50.0f,
	50.0f,
	0.0f,
};
static const std::array<bool, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_AUTOMATABLE {
	true,
	true,
	true,
};
static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_NICKNAMES{
	"Input Gain",
	"Output Gain",
	"Index",
};
static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_SUFFIXES {
	"%",
	"%",
	"",
};
static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_TOOLTIPS {
	"Input Loudness Parameter",
	"Output Loudness Parameter",
	"Wavetable Index",
};
static const std::array<std::vector<juce::String>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_TO_STRING_ARRS {
	std::vector<juce::String>{
	},
	std::vector<juce::String>{
	},
	std::vector<juce::String>{
	},
};