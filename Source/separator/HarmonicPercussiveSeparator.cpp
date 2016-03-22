/*
  ==============================================================================

    HarmonicPercussiveSeparator.cpp
    Created: 22 Mar 2016 1:00:45pm
    Author:  Sean

  ==============================================================================
*/

#include "HarmonicPercussiveSeparator.h"

MedianSeparator::MedianSeparator(AudioFormatReader* fileReader) : reader(fileReader)
{
	
}

MedianSeparator::~MedianSeparator()
{}

void MedianSeparator::fillBuffer()
{
	numChannels = reader->numChannels;
	numSamples = reader->lengthInSamples;
	numCols = 1 + floor((numSamples - WINDOW_SIZE) / HOP_SIZE);
	startSample = 0;
	int readerStartSample = 0;

	samples.setSize(numChannels, numSamples);
	samples.clear();

	reader->read(&samples, startSample, numSamples, readerStartSample, true, true);
}

void MedianSeparator::convertToSpectrogram()
{
	ScopedPointer<STFT> stft = new STFT(WINDOW_SIZE);
	stft->initWindow(1);	// initialise hann window

	float* bufferData[2];
	std::complex<float>* spectrogramData[2];

	for (int i = 0; i < 2; i++)
	{
		// initialise structures to hold audio data in different forms
		bufferData[i] = new float[WINDOW_SIZE];
		spectrogram[i] = MatrixXcf::Zero((WINDOW_SIZE / 2) + 1, numCols);
		spectrogramData[i] = new std::complex<float>[(WINDOW_SIZE/2) + 1];
	}

	for (int column = 0; column < numCols; column++)
	{
		for (int i = 0; i < 2; i++)
		{
			bufferData[i] = (float*)samples.getReadPointer(i, startSample);

			float* fftData = stft->performForwardTransform(bufferData[i]);
			spectrogramData[i] = stft->realToComplex(fftData, WINDOW_SIZE);

			for (int sample = 0; sample < 2049; sample++)
			{
				spectrogram[i](sample, column) = spectrogramData[i][sample];
			}
		}

		startSample += HOP_SIZE;
	}

}

void MedianSeparator::filterBins()
{
	for (int channel = 0; channel < 2; channel++)
	{
		filteredSpectrogram_H[channel] = MatrixXf::Zero((WINDOW_SIZE / 2) + 1, numCols);
	}

	/*
	int numRows = spectrogram[0].rows();

	for (int row = 0; row < numRows; row++)	// loop through rows in spectrogram
	{
		for (int col = 0; col < numCols; col++)		// loop through columns in spectrogram
		{
			Array<float> binData[2];
			MyArraySorter arraySorter;
			float median;

			for (int sample = 0; sample < (col + FILTER_SIZE); sample++)
			{
				if (sample < spectrogram[0].cols())
				{
					for (int channel = 0; channel < 2; channel++)
					{
						binData[channel].add(std::abs(spectrogram[channel](row, sample)));
					}

					if (binData[0].size() == FILTER_SIZE) // filter batch is full 
					{
						for (int channel = 0; channel < 2; channel++)
						{
							binData[channel].sort(arraySorter);
							median = binData[channel][(FILTER_SIZE / 2) + 1];
							filteredSpectrogram_H[channel](row, col) = median;
						}
					}
				}
			}
		}
	}
	*/

	int numRows = spectrogram[0].rows();

	// loop through rows of spectrogram
	for (int row = 0; row < numRows; row++)
	{

		// loop through columns of spectrogram
		for (int col = 0; col < spectrogram[0].cols(); col++)
		{
			Array<float> binData[2];
			MyArraySorter arraySorter;
			float median;

			for (int samp = col; samp < (col + FILTER_SIZE); samp++)
			{
				if (samp < spectrogram[0].cols())
				{
					binData[0].add(std::abs(spectrogram[0](row, samp)));
					binData[1].add(std::abs(spectrogram[1](row, samp)));
				}

				if (binData[0].size() == FILTER_SIZE) // when all samples in filter batch
				{
					// sort filter batch
					binData[0].sort(arraySorter);
					binData[1].sort(arraySorter);

					// assign median value to filteredSpectro_Harm[0](row, col)
					median = binData[0][MEDIAN_INDEX];
					filteredSpectrogram_H[0](row, col) = median;

					// assign median value to filteredSpectro_Harm[1](row, col)
					median = binData[1][MEDIAN_INDEX];
					filteredSpectrogram_H[1](row, col) = median;

				}
			}
		}
	}
}

void MedianSeparator::filterFrames()
{
	for (int channel = 0; channel < 2; channel++)
	{
		filteredSpectrogram_P[channel] = MatrixXf::Zero((WINDOW_SIZE / 2) + 1, numCols);
	}

	int numCols = spectrogram[0].cols();

	// loop columns
	for (int col = 0; col < numCols; col++)
	{

		// loop rows
		for (int row = 0; row < spectrogram[0].rows(); row++)
		{

			Array<float> frameData[2];
			MyArraySorter arraySorter;
			float median;

			for (int samp = row; samp < (row + FILTER_SIZE); samp++)
			{
				if (samp < 2049)
				{
					frameData[0].add(std::abs(spectrogram[0](samp, col)));
					frameData[1].add(std::abs(spectrogram[1](samp, col)));
				}

				if (frameData[0].size() == FILTER_SIZE) // when all samples in filter batch
				{
					// sort arrays
					frameData[0].sort(arraySorter);
					frameData[1].sort(arraySorter);

					// assign median value to filteredSpectro_Perc[0](row, col)
					median = frameData[0][MEDIAN_INDEX];
					filteredSpectrogram_P[0](row, col) = median;

					// assign median value to filteredSpectro_Perc[1](row, col)
					median = frameData[1][MEDIAN_INDEX];
					filteredSpectrogram_P[1](row, col) = median;
				}
			}
		}
	}
}

void MedianSeparator::resynthesize()
{
	MatrixXf pest2[2];
	pest2[0] = filteredSpectrogram_P[0].cwiseProduct(filteredSpectrogram_P[0]);
	pest2[1] = filteredSpectrogram_P[1].cwiseProduct(filteredSpectrogram_P[1]);

	MatrixXf hest2[2];
	hest2[0] = filteredSpectrogram_H[0].cwiseProduct(filteredSpectrogram_H[0]);
	hest2[1] = filteredSpectrogram_H[1].cwiseProduct(filteredSpectrogram_H[1]);

	MatrixXf maskDivisorL = (pest2[0] + hest2[0]);
	MatrixXf maskDivisorR = (pest2[1] + hest2[1]);

	MatrixXf p_maskL = pest2[0].cwiseQuotient(maskDivisorL);
	MatrixXf p_maskR = pest2[1].cwiseQuotient(maskDivisorR);

	MatrixXf h_maskL = hest2[0].cwiseQuotient(maskDivisorL);
	MatrixXf h_maskR = hest2[1].cwiseQuotient(maskDivisorR);

	resynthSpectrogram_P[0] = spectrogram[0].cwiseProduct(p_maskL);
	resynthSpectrogram_P[1] = spectrogram[1].cwiseProduct(p_maskR);

	resynthSpectrogram_H[0] = spectrogram[0].cwiseProduct(h_maskL);
	resynthSpectrogram_H[1] = spectrogram[1].cwiseProduct(h_maskR);
}

void MedianSeparator::writeFiles()
{
	ScopedPointer<ISTFT> istft = new ISTFT();
	istft->initWindow(1);

	float ifftResults_Left[WINDOW_SIZE], ifftResults_Right[WINDOW_SIZE];
	float temp_L[WINDOW_SIZE] = {};
	float temp_R[WINDOW_SIZE] = {};

	Eigen::MatrixXf realMatrix_Left, realMatrix_Right;
	realMatrix_Left = MatrixXf::Zero(WINDOW_SIZE, numCols);
	realMatrix_Right = MatrixXf::Zero(WINDOW_SIZE, numCols);

	realMatrix_Left = istft->complexToReal(resynthSpectrogram_H[0]);
	realMatrix_Right = istft->complexToReal(resynthSpectrogram_H[1]);

	for (int i = 0; i < numSamples; i++)
	{
		outputSignal_Left.set(i, 0.0f);
		outputSignal_Right.set(i, 0.0f);
	}

	// add-overlap ====================
	int offset = 0;
	for (int col = 0; col < numCols; col++)
	{
		for (int row = 0; row < WINDOW_SIZE; row++)
		{
			temp_L[row] = realMatrix_Left(row, col);
			temp_R[row] = realMatrix_Right(row, col);
		}

		istft->performInverseTransform(temp_L, ifftResults_Left);
		istft->rescale(ifftResults_Left);
		istft->performInverseTransform(temp_R, ifftResults_Right);
		istft->rescale(ifftResults_Right);

		for (int i = 0; i < WINDOW_SIZE; i++)
		{
			outputSignal_Left.set(offset + i, (outputSignal_Left[offset + i] + (ifftResults_Left[i] * istft->window[i])));
			outputSignal_Right.set(offset + i, (outputSignal_Right[offset + i] + (ifftResults_Right[i] * istft->window[i])));
		}

		offset += HOP_SIZE;
	}

	// TODO
	float gain = 0.5f;
	juce::AudioSampleBuffer outSamples(2, numSamples);
	outSamples.clear();
	const float* leftData = outputSignal_Left.getRawDataPointer();
	const float* rightData = outputSignal_Right.getRawDataPointer();

	outSamples.addFrom(0, 0, leftData, numSamples, gain);
	outSamples.addFrom(1, 0, rightData, numSamples, gain);

	FileOutputStream* output;
	File* outputFile = new File(File::getCurrentWorkingDirectory().getChildFile(/*fileNameNoExt + "_perc" + fileExtension*/ "test.wav"));

	if (outputFile->exists())
	{
		outputFile->deleteFile();
	}

	output = outputFile->createOutputStream();
	WavAudioFormat* wavFormat = new WavAudioFormat();
	AudioFormatWriter* writer = wavFormat->createWriterFor(output, 44100.0, numChannels, 16, NULL, 0);

	// write from sample buffer
	writer->flush();
	writer->writeFromAudioSampleBuffer(outSamples, 0, numSamples);
	writer->flush();

	// cleanup
	delete writer;
	delete wavFormat;
	wavFormat = nullptr;
	writer = nullptr;
	outputSignal_Left.clearQuick();
	outputSignal_Right.clearQuick();
}