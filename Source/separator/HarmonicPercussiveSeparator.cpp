/*
  ==============================================================================

    HarmonicPercussiveSeparator.cpp
    Created: 22 Mar 2016 1:00:45pm
    Author:  Sean

	This class performs the separation on the audio data, read from 
	the AudioFormatReader object.

  ==============================================================================
*/

#include "HarmonicPercussiveSeparator.h"

MedianSeparator::MedianSeparator(AudioFormatReader* fileReader) : reader(fileReader)
{
	progress = 0.0;
}

MedianSeparator::~MedianSeparator()
{}

void MedianSeparator::fillBuffer()
{
	// num of Channels in audio file
	numChannels = reader->numChannels;

	// length of audio file in samples
	numSamples = reader->lengthInSamples;

	// calculate number of columns using FFT size (4096) and HOP_SIZE (1024)
	numCols = 1 + floor((numSamples - WINDOW_SIZE) / HOP_SIZE);

	startSample = 0;				// start sample in destination buffer
	int readerStartSample = 0;		// start sample in reader

	samples.setSize(numChannels, numSamples);
	samples.clear();				// clear buffer to remove any junk

	// start reading samples from reader into samples
	reader->read(&samples, startSample, numSamples, readerStartSample, true, true);

}

void MedianSeparator::convertToSpectrogram()
{
	// create STFT object to perform FFT
	ScopedPointer<STFT> stft = new STFT(WINDOW_SIZE);

	stft->initWindow(1);	// initialise hann window

	// 2d array to hold pointer to sample data
	float* bufferData[2];	

	// 2d array of complex floats to hold complex spectrogram data after fft
	std::complex<float>* spectrogramData[2];	

	for (int i = 0; i < 2; i++)
	{
		// initialise structures to hold audio data in different forms
		bufferData[i] = new float[WINDOW_SIZE];
		spectrogram[i] = MatrixXcf::Zero((WINDOW_SIZE / 2) + 1, numCols); // holds entire spectrogram
		spectrogramData[i] = new std::complex<float>[(WINDOW_SIZE/2) + 1];
	}

	// loop through each column of spectrogram
	for (int column = 0; column < numCols; column++)
	{
		// for each channel
		for (int i = 0; i < numChannels; i++)
		{
			// get pointer to sample data
			bufferData[i] = (float*)samples.getReadPointer(i, startSample);

			// perform FFT
			float* fftData = stft->performForwardTransform(bufferData[i]);

			// convert from real to complex spectum data
			spectrogramData[i] = stft->realToComplex(fftData, WINDOW_SIZE);

			// fill column in spectrogram with complex data from FFT
			for (int sample = 0; sample < 2049; sample++)
			{
				spectrogram[i](sample, column) = spectrogramData[i][sample];
			}
		}

		// increment beginning sample by HOP_SIZE (1024)
		startSample += HOP_SIZE;
	}

}

void MedianSeparator::filterBins()
{
	// clear matrix of any data
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

			// loop through samples
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
	/*

	Pest = filteredSpectrogram_P
	Hest = filteredSpectrogram_H

	Masks for audio:

	P = OriginalSpectrogram x Pest^2 / (Pest^2 + Hest^2)

	H = OriginalSpectrogram x Hest^2 / (Pest^2 + Hest^2)

	where
	Pest = spectrogram for filtered frame data
	Hest = spectrogram for filtered frequency bin data

	*/
 
	// Pest^2
	MatrixXf pest2[2];
	pest2[0] = filteredSpectrogram_P[0].cwiseProduct(filteredSpectrogram_P[0]);
	pest2[1] = filteredSpectrogram_P[1].cwiseProduct(filteredSpectrogram_P[1]);

	// Hest^2
	MatrixXf hest2[2];
	hest2[0] = filteredSpectrogram_H[0].cwiseProduct(filteredSpectrogram_H[0]);
	hest2[1] = filteredSpectrogram_H[1].cwiseProduct(filteredSpectrogram_H[1]);

	// (Pest^2 + Hest^2)
	MatrixXf maskDivisorL = (pest2[0] + hest2[0]);
	MatrixXf maskDivisorR = (pest2[1] + hest2[1]);
	
	// Pest^2 / (Pest^2 + Hest^2)
	MatrixXf p_maskL = pest2[0].cwiseQuotient(maskDivisorL);
	MatrixXf p_maskR = pest2[1].cwiseQuotient(maskDivisorR);

	// Hest^2 / (Pest^2 + Hest^2)
	MatrixXf h_maskL = hest2[0].cwiseQuotient(maskDivisorL);
	MatrixXf h_maskR = hest2[1].cwiseQuotient(maskDivisorR);

	// OriginalSpectrogram x Pest^2 / (Pest^2 + Hest^2)
	resynthSpectrogram_P[0] = spectrogram[0].cwiseProduct(p_maskL);
	resynthSpectrogram_P[1] = spectrogram[1].cwiseProduct(p_maskR);

	// OriginalSpectrogram x Hest^2 / (Pest^2 + Hest^2)
	resynthSpectrogram_H[0] = spectrogram[0].cwiseProduct(h_maskL);
	resynthSpectrogram_H[1] = spectrogram[1].cwiseProduct(h_maskR);
}

void MedianSeparator::writeFiles()
{

	ScopedPointer<ISTFT> istft = new ISTFT();
	istft->initWindow(1);

	// Matrices to hold real number data after conversion
	Eigen::MatrixXf harmonicSpectrogramReal_Left;
	Eigen::MatrixXf harmonicSpectrogramReal_Right;
	Eigen::MatrixXf percussiveSpectrogramReal_Left;
	Eigen::MatrixXf percussiveSpectrogramReal_Right;

	// initialise with zeros
	harmonicSpectrogramReal_Left = MatrixXf::Zero(WINDOW_SIZE, numCols);
	harmonicSpectrogramReal_Right = MatrixXf::Zero(WINDOW_SIZE, numCols);
	percussiveSpectrogramReal_Left = MatrixXf::Zero(WINDOW_SIZE, numCols);
	percussiveSpectrogramReal_Right = MatrixXf::Zero(WINDOW_SIZE, numCols);

	// convert complex spectrogram data to real spectrogram data
	harmonicSpectrogramReal_Left = istft->complexToReal(resynthSpectrogram_H[0]);
	harmonicSpectrogramReal_Right = istft->complexToReal(resynthSpectrogram_H[1]);
	percussiveSpectrogramReal_Left = istft->complexToReal(resynthSpectrogram_P[0]);
	percussiveSpectrogramReal_Right = istft->complexToReal(resynthSpectrogram_P[1]);

	// arrays to hold output signals for harmonic and percussive files
	Array<float> outputSignal_P[2];
	Array<float> outputSignal_H[2];

	// fill with zeros (faster than looping through)
	outputSignal_P[0].insertMultiple(0, 0.0f, numSamples);
	outputSignal_P[1].insertMultiple(0, 0.0f, numSamples);
	outputSignal_H[0].insertMultiple(0, 0.0f, numSamples);
	outputSignal_H[1].insertMultiple(0, 0.0f, numSamples);

	// add-overlap ====================
	int offset = 0;
	float temp_L[WINDOW_SIZE] = {};
	float temp_R[WINDOW_SIZE] = {};
	float ifftResults_Left[WINDOW_SIZE] = {};
	float ifftResults_Right[WINDOW_SIZE] = {};

	// loop through each column in spectrograms
	for (int col = 0; col < numCols; col++)
	{
		// insert 4096 samples into temp arrays from harmonic spectrogram
		for (int row = 0; row < WINDOW_SIZE; row++)
		{
			temp_L[row] = harmonicSpectrogramReal_Left(row, col);
			temp_R[row] = harmonicSpectrogramReal_Right(row, col);
		}

		// inverse short time fourier transform on temp_L and temp_R
		istft->performInverseTransform(temp_L, ifftResults_Left);
		istft->rescale(ifftResults_Left);
		istft->performInverseTransform(temp_R, ifftResults_Right);
		istft->rescale(ifftResults_Right);

		// set values in output signal arrays for Harmonic output signal
		for (int i = 0; i < WINDOW_SIZE; i++)
		{
			outputSignal_H[0].set(offset + i, (outputSignal_H[0][offset + i] + (ifftResults_Left[i] * istft->window[i])));
			outputSignal_H[1].set(offset + i, (outputSignal_H[1][offset + i] + (ifftResults_Right[i] * istft->window[i])));
		}

		// insert 4096 samples into temp arrays from percussive spectrogram
		for (int row = 0; row < WINDOW_SIZE; row++)
		{
			temp_L[row] = percussiveSpectrogramReal_Left(row, col);
			temp_R[row] = percussiveSpectrogramReal_Right(row, col);
		}

		// inverse short time fourier transform on temp arrays
		istft->performInverseTransform(temp_L, ifftResults_Left);
		istft->rescale(ifftResults_Left);
		istft->performInverseTransform(temp_R, ifftResults_Right);
		istft->rescale(ifftResults_Right);

		// set values in output arrays for Percussive output signal
		for (int i = 0; i < WINDOW_SIZE; i++)
		{
			outputSignal_P[0].set(offset + i, (outputSignal_P[0][offset + i] + (ifftResults_Left[i] * istft->window[i])));
			outputSignal_P[1].set(offset + i, (outputSignal_P[1][offset + i] + (ifftResults_Right[i] * istft->window[i])));
		}

		offset += HOP_SIZE;	// increment offset by HOP_SIZE (1024)
	}

	//===================================================== WRITE FILES ==
	float gain = 0.5f; // 1.0f divided by num of output files (2)

	AudioSampleBuffer outSamples_H(2, numSamples);
	AudioSampleBuffer outSamples_P(2, numSamples);

	outSamples_H.clear();
	outSamples_P.clear();

	const float* leftData_H = outputSignal_H[0].getRawDataPointer();
	const float* rightData_H = outputSignal_H[1].getRawDataPointer();
	const float* leftData_P = outputSignal_P[0].getRawDataPointer();
	const float* rightData_P = outputSignal_P[1].getRawDataPointer();

	outSamples_H.addFrom(0, 0, leftData_H, numSamples, gain);
	outSamples_H.addFrom(1, 0, rightData_H, numSamples, gain);

	outSamples_P.addFrom(0, 0, leftData_P, numSamples, gain);
	outSamples_P.addFrom(1, 0, rightData_P, numSamples, gain);

	File* outputFile_H = new File(File::getCurrentWorkingDirectory().getChildFile(fileNameNoExt + "_harmonic.wav"));
	File* outputFile_P = new File(File::getCurrentWorkingDirectory().getChildFile(fileNameNoExt + "_percussive.wav"));
	
	if (outputFile_H->exists())
	{
		outputFile_H->deleteFile();
	}

	if (outputFile_P->exists())
	{
		outputFile_P->deleteFile();
	}

	FileOutputStream* output_H;
	FileOutputStream* output_P;

	output_H = outputFile_H->createOutputStream();
	output_P = outputFile_P->createOutputStream();

	// write from sample buffer
	WavAudioFormat* wavFormat = new WavAudioFormat();
	AudioFormatWriter* writer = wavFormat->createWriterFor(output_H, 44100.0, numChannels, 16, NULL, 0);
	writer->flush();
	writer->writeFromAudioSampleBuffer(outSamples_H, 0, numSamples);
	delete writer;

	writer = wavFormat->createWriterFor(output_P, 44100.0, numChannels, 16, NULL, 0);
	writer->flush();
	writer->writeFromAudioSampleBuffer(outSamples_P, 0, numSamples);

	// cleanup
	delete writer;
	delete wavFormat;
	wavFormat = nullptr;
	writer = nullptr;

	outSamples_H.clear();
	outSamples_P.clear();
	outputSignal_H[0].clear();
	outputSignal_H[1].clear();
	outputSignal_P[0].clear();
	outputSignal_P[1].clear();

}

double MedianSeparator::getProgress()
{
	return progress;
}