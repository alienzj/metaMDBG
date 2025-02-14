

#ifndef MDBG_METAG_ASSEMBLYPIPELINE
#define MDBG_METAG_ASSEMBLYPIPELINE

#include "../Commons.hpp"


class AssemblyPipeline : public Tool{
    
public:

	string _inputFilename;
	string _outputDir;
	float _minimizerDensity;
    size_t _minimizerSize;
    size_t _kminmerSize;
	string _filename_readMinimizers;
	string _filename_exe;
	string _truthInputFilename;
	int _nbCores;
	bool _useInitialKminmerFilter;

	size_t _firstK;
	size_t _lastK;
	u_int64_t _maxAssemblyLength;

	string _inputFilenameComplete;
	size_t _meanReadLength;
	string _tmpDir;

	u_int64_t _contigPolishing_nbReadFragments;
	u_int64_t _strainPurging_minContigLength;
	double _strainPurging_minIdentity;

	AssemblyPipeline(): Tool (){
	}

    void execute (){

		ofstream fileLogs(_tmpDir + "/logs.txt");
		fileLogs.close();

		//const string& failedFilename = _tmpDir + "/failed.txt";
		//if(fs::exists(failedFilename)){
		//	fs::remove(_tmpDir + "/failed.txt");
		//}

		openLogFile(_tmpDir);
		checkDependencies();

		auto start = high_resolution_clock::now();

		//for(size_t i=0; i<1000; i++)
		execute_pipeline();

		auto stop = high_resolution_clock::now();


		/*
		string maxMemStr = "Peakmemory(GB):";
		ifstream infile(_tmpDir + "/perf.txt");
		string line;
		double peakMemory = 0;

		while(std::getline(infile, line)){
			//cout << line << endl;
    		//line.erase(std::remove_if(line.begin(), line.end(), ::istab), line.end());
    		line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
			if(line.empty()) continue;

			size_t pos = line.find(maxMemStr);
			if (pos != std::string::npos){
				line.erase(pos, maxMemStr.length());
			}
			else{
				continue;
			}


			peakMemory = stod(line);
			break;
		}
		infile.close();
		*/

		//cout << "Duration: " << duration_cast<seconds>(stop - start).count() << "s" << endl;
		ofstream outfile(_tmpDir + "/perf.txt", std::ios_base::app);
		outfile << "Run time (hh:mm:ss): " << Utils::formatTimeToHumanReadable(duration_cast<seconds>(stop - start)) << endl;
		outfile.close();

		ifstream perfFile(_tmpDir + "/perf.txt");

		cerr << endl;
		cerr <<  perfFile.rdbuf();
		//cerr << "Peak memory (GB)    : " << peakMemory << endl;
		//cerr << "Run time (hh:mm:ss) : " << Utils::formatTimeToHumanReadable(duration_cast<seconds>(stop - start)) << endl;
		cerr << "Contig filename: " << _outputDir + "/contigs.fasta.gz" << endl;
		cerr << "done!" << endl;

		perfFile.close();
	}

	void parseArgs(int argc, char* argv[]){

		args::ArgumentParser parser("asm", ""); //"This is a test program.", "This goes after the options."
		args::Positional<std::string> arg_outputDir(parser, "outputDir", "Output dir for contigs and temporary files", args::Options::Required);
		args::PositionalList<std::string> arg_readFilenames(parser, "reads", "Read filename(s) (separated by space)", args::Options::Required);
		args::ValueFlag<int> arg_l(parser, "", "Minimizer length", {ARG_MINIMIZER_LENGTH2}, 13);
		args::ValueFlag<float> arg_d(parser, "", "Minimizer density", {ARG_MINIMIZER_DENSITY2}, 0.005f);
		args::ValueFlag<int> arg_maxK(parser, "", "Stop assembly when reaching this k-mer length in bps (e.g. 5000)", {ARG_MAXK}, 0);
		args::ValueFlag<int> arg_nbWindows(parser, "", "Maximum read coverage used for contig correction (increase for better correction)", {ARG_NB_WINDOWS}, 50);
		args::ValueFlag<int> arg_length(parser, "", "Use contigs with length > l as references for strain duplication purging", {ARG_LINEAR_LENGTH}, 1000000);
		args::ValueFlag<float> arg_minIdentity(parser, "", "Minimum identity for strain purging (0-1)", {ARG_MIN_IDENTITY}, 0.99);
		args::Flag arg_bf(parser, "", "Disable unique kminmer filter prior to graph construction", {ARG_BLOOM_FILTER});
		args::ValueFlag<int> arg_nbCores(parser, "", "Number of cores", {ARG_NB_CORES2}, NB_CORES_DEFAULT_INT);
		args::Flag arg_help(parser, "", "", {'h', "help"}, args::Options::Hidden);
		//args::HelpFlag help(parser, "help", "Display this help menu", {'h'});
		//args::CompletionFlag completion(parser, {"complete"});

		//(ARG_INPUT_FILENAME_TRUTH, "", cxxopts::value<string>()->default_value(""))
		//(ARG_MINIMIZER_LENGTH, "", cxxopts::value<int>()->default_value("13"))
		//(ARG_MINIMIZER_DENSITY, "", cxxopts::value<float>()->default_value("0.005"))
		//(ARG_NB_CORES, "", cxxopts::value<int>()->default_value(NB_CORES_DEFAULT))
		//(ARG_BLOOM_FILTER, "", cxxopts::value<bool>()->default_value("false"));

		try
		{
			parser.ParseCLI(argc, argv);
		}
		catch (const args::Help&)
		{
			std::cerr << parser;
			exit(0);
		}
		catch (const std::exception& e)
		{
			std::cerr << parser;
			//cout << endl;
			std::cerr << e.what() << endl;
			exit(0);
		}

		if(arg_help){
			std::cerr << parser;
			exit(0);
		}

		_outputDir = args::get(arg_outputDir);
		_tmpDir = _outputDir + "/tmp/";
		_minimizerSize = args::get(arg_l);
		_minimizerDensity = args::get(arg_d);
		_nbCores = args::get(arg_nbCores);

		_maxAssemblyLength = 0;
		if(arg_maxK){
			_maxAssemblyLength = args::get(arg_maxK);
		}

		_contigPolishing_nbReadFragments = args::get(arg_nbWindows);
		_strainPurging_minContigLength = args::get(arg_length);
		_strainPurging_minIdentity = args::get(arg_minIdentity);
		_strainPurging_minIdentity = min(_strainPurging_minIdentity, 0.999); //Some duplication is required to remove erroneous contigs

		_useInitialKminmerFilter = true;
		if(arg_bf){
			_useInitialKminmerFilter = false;
		}



	    if(!fs::exists (_outputDir)) fs::create_directories(_outputDir); 
	    if(!fs::exists (_tmpDir)) fs::create_directories(_tmpDir); 
	    if(!fs::exists (_tmpDir + "/filter")) fs::create_directories(_tmpDir + "/filter"); 
		
		
		//string bannedDir = _tmpDir + "/bannedKminmers/";
	    //if(fs::exists (bannedDir)){
		//	fs::remove_all(bannedDir);
		//}
		//fs::create_directories(bannedDir); 

		_inputFilename = _tmpDir + "/input.txt";
		Commons::createInputFile(args::get(arg_readFilenames), _inputFilename);
		//if (arg_l) { 
		//}

		/*
		catch (const args::Help&)
		{
			std::cout << parser;
			exit(0);
		}
		catch (const args::ParseError& e)
		{
			//std::cerr << e.what() << std::endl;
			//std::cerr << parser;
			std::cout << parser;
			std::exit(EXIT_FAILURE);
		}
		*/
		//return 0;

		/*
		cxxopts::Options options("AssemblyPipeline", "");
		options.add_options()
		//("d,debug", "Enable debugging") // a bool parameter
		("reads", "", cxxopts::value<string>())
		//("asmDir", "", cxxopts::value<string>())
		("outputDir", "", cxxopts::value<string>())
		//(ARG_INPUT_FILENAME, "", cxxopts::value<string>())
		//(ARG_OUTPUT_DIR, "", cxxopts::value<string>())
		(ARG_INPUT_FILENAME_TRUTH, "", cxxopts::value<string>()->default_value(""))
		(ARG_MINIMIZER_LENGTH, "", cxxopts::value<int>()->default_value("13"))
		(ARG_MINIMIZER_DENSITY, "", cxxopts::value<float>()->default_value("0.005"))
		(ARG_NB_CORES, "", cxxopts::value<int>()->default_value(NB_CORES_DEFAULT))
		(ARG_BLOOM_FILTER, "", cxxopts::value<bool>()->default_value("false"));
		//(ARG_KMINMER_LENGTH, "", cxxopts::value<int>()->default_value("3"))
		//("k,kminmerSize", "File name", cxxopts::value<std::string>())
		//("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
		//;

		//(ARG_INPUT_FILENAME_TRUTH, "", cxxopts::value<string>()->default_value(""))
		//(ARG_EVAL, "", cxxopts::value<bool>()->default_value("false"))
		//(ARG_INPUT_FILENAME_ABUNDANCE, "", cxxopts::value<string>()->default_value(""))
		//(ARG_NB_CORES, "", cxxopts::value<int>()->default_value("8"));
		//(ARG_KMINMER_LENGTH, "", cxxopts::value<int>()->default_value("3"))
		//(ARG_MINIMIZER_LENGTH, "", cxxopts::value<int>()->default_value("21"))
		//(ARG_MINIMIZER_DENSITY, "", cxxopts::value<float>()->default_value("0.005"));
		//("k,kminmerSize", "File name", cxxopts::value<std::string>())
		//("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
		//;
		options.parse_positional({"reads", "outputDir"});
		options.positional_help("reads outputDir");

		if(argc <= 1){
			std::cout << options.help() << std::endl;
			exit(0);
		}


		cxxopts::ParseResult result;

		try{
			result = options.parse(argc, argv);


			_inputFilename = result["reads"].as<string>();
			//_tmpDir = result["asmDir"].as<string>();
			//_outputDir_binning = result["outputDir"].as<string>() + "/bins_/";

			//_kminmerSize = result[ARG_KMINMER_LENGTH].as<int>(); //getInput()->getInt(STR_KMINMER_SIZE);
			//_inputFilename = result[ARG_INPUT_FILENAME].as<string>(); //getInput()->getStr(STR_INPUT);
			_tmpDir = result["outputDir"].as<string>(); //getInput()->getStr(STR_OUTPUT);
			_truthInputFilename = result[ARG_INPUT_FILENAME_TRUTH].as<string>();
			_nbCores = result[ARG_NB_CORES].as<int>();
			_minimizerSize = result[ARG_MINIMIZER_LENGTH].as<int>(); //getInput()->getInt(STR_MINIM_SIZE);
			_minimizerDensity = result[ARG_MINIMIZER_DENSITY].as<float>(); //getInput()->getDouble(STR_DENSITY);
			_useBloomFilter = result[ARG_BLOOM_FILTER].as<bool>();

		}
		catch (const std::exception& e){
			std::cout << options.help() << std::endl;
			std::cerr << e.what() << std::endl;
			std::exit(EXIT_FAILURE);
		}
		*/



		//T exception{text};




		//_inputFilename = result[STR_INPUT].as<string>();

		//_compositionManager = new CompositionManager(4);

		//int w = 80;
		


		//if(!System::file().doesExist (_outputDir)) System::file().mkdir(_outputDir, -1);
		//createInputFile();

		cerr << endl;
		cerr << "Input: " << _inputFilename << endl;
		cerr << "Dir: " << _outputDir << endl;
		cerr << "Minimizer length: " << _minimizerSize << endl;
		//cout << "Kminmer length: " << _kminmerSize << endl;
		cerr << "Density: " << _minimizerDensity << endl;
		cerr << endl;

		_logFile << endl;
		_logFile << "Input: " << _inputFilename << endl;
		_logFile << "Dir: " << _outputDir << endl;
		_logFile << "Minimizer length: " << _minimizerSize << endl;
		//cout << "Kminmer length: " << _kminmerSize << endl;
		_logFile << "Density: " << _minimizerDensity << endl;
		_logFile << endl;

		_filename_exe = argv[0];
		/*
		//_outputDir = getInput()->get(STR_INPUT_DIR) ? getInput()->getStr(STR_INPUT_DIR) : "";
		//_input_extractKminmers= getInput()->get(STR_INPUT_EXTRACT_KMINMERS) ? getInput()->getStr(STR_INPUT_EXTRACT_KMINMERS) : "";

		_filename_readMinimizers = _outputDir + "/read_data.gz";
		//_filename_readCompositions = _outputDir + "/read_compositions.gz";
		//_filename_filteredMinimizers = _outputDir + "/filteredMinimizers.gz";
		//_filename_hifiasmGroundtruth = _outputDir + "/hifiasmGroundtruth.gz";


		string filename_parameters = _outputDir + "/parameters.gz";
		gzFile file_parameters = gzopen(filename_parameters.c_str(),"wb");
		gzwrite(file_parameters, (const char*)&_minimizerSize, sizeof(_minimizerSize));
		gzwrite(file_parameters, (const char*)&_kminmerSize, sizeof(_kminmerSize));
		gzwrite(file_parameters, (const char*)&_minimizerDensity, sizeof(_minimizerDensity));
		gzclose(file_parameters);
		*/

		//string filename = _outputDir + "/" + FILENAME_NO_KMINMER_READS;
		//if(fs::exists(filename)){
		//	fs::remove(filename);
		//}
		
	}



    void execute_pipeline(){



		string command = "";
		writeParameters(_minimizerSize, _firstK, _minimizerDensity, _firstK, _firstK, _lastK);
		//createInputFile(false);

		cerr << "Converting reads to minimizers..." << endl;
		_logFile << "Converting reads to minimizers..." << endl;

		//Read selection
		command = _filename_exe + " readSelection " + _tmpDir + " " + _tmpDir + "/read_data_init.txt" + " " + _inputFilename + " -t " + to_string(_nbCores);
		executeCommand(command);









		//float density = 0.005;
		//u_int16_t minimizerSize = 21;

		_firstK = 4;

		ifstream file_readStats(_tmpDir + "/read_stats.txt");
	
		u_int32_t n50ReadLength;
		file_readStats.read((char*)&n50ReadLength, sizeof(n50ReadLength));

		file_readStats.close();


		//u_int64_t meanReadLength = computeMeanReadLength(_inputFilename);
		_lastK = n50ReadLength*_minimizerDensity*2.0f; //1.2f; //2.0f; //*0.95
		_meanReadLength = n50ReadLength;
		//_lastK = 10;

		if(_maxAssemblyLength > 0){
			int minimizerSize = _minimizerSize;
			int maxAssemblyLength = _maxAssemblyLength;
			int maxK = ((maxAssemblyLength-minimizerSize) * _minimizerDensity) + 1;;
			_lastK = max(0, maxK);
		}

		_lastK = max(_lastK, _firstK+1);

		/*
		sort(_readLengths.begin(), _readLengths.end());
		_logFile << _readLengths[_readLengths.size() * 0.1] << endl;
		_logFile << _readLengths[_readLengths.size() * 0.2] << endl;
		_logFile << _readLengths[_readLengths.size() * 0.3] << endl;
		_logFile << _readLengths[_readLengths.size() * 0.4] << endl;
		_logFile << _readLengths[_readLengths.size() * 0.5] << endl;
		_logFile << _readLengths[_readLengths.size() * 0.6] << endl;
		_logFile << _readLengths[_readLengths.size() * 0.7] << endl;
		_logFile << _readLengths[_readLengths.size() * 0.8] << endl;
		_logFile << _readLengths[_readLengths.size() * 0.9] << endl;
		_readLengths.clear();
		//return scores[size * 0.1];
		*/

		
		//cerr << "Mean read length: " << meanReadLength << endl;
		cerr << "N50 read length: " << n50ReadLength << endl;
		cerr << "Min k: " << _firstK << endl;
		cerr << "Max k: " << _lastK << endl;
		cerr << endl;
		_logFile << "N50 read length: " << n50ReadLength << endl;
		_logFile << "Min k: " << _firstK << endl;
		_logFile << "Max k: " << _lastK << endl;
		_logFile << endl;



		ofstream fileSmallContigs(_tmpDir + "/small_contigs.bin");
		fileSmallContigs.close();
		//ofstream fileJoints(_tmpDir + "/joint_data.txt");
		//fileJoints.close();


		

		u_int64_t pass = 0;
		u_int32_t prevK = -1;

		//for(size_t k=_firstK; k<_lastK; k+=1){

		size_t k = _firstK;

		while(k < _lastK){


			cerr << "Multi-k pass: " << k << "/" << _lastK << endl;
			_logFile << "Multi-k pass: " << k << "/" << _lastK << endl;
			//cout << "Start asm: " << k << endl;


			//if(pass > 0) createInputFile(true);

			//if(pass <= 0){
			//	prevK = k;
			//	pass += 1;
			//	continue;
			//}

			//string inputFilename = "";
			//if(pass == 0){
			//	inputFilename = _inputFilename;
			//}
			//else{
			//	inputFilename = createInputFile(prevK);
			//}

			//Read selection
			//command = _filename_exe + " readSelection -i " + inputFilename + " -o " + _tmpDir + " -f " + _tmpDir + "/read_data.gz";
			//if(pass == 0) command += " --firstpass";
			//executeCommand(command);

			executePass(k, prevK, pass);

		
			//./bin/mdbgAsmMeta bin -o ~/workspace/run/overlap_test_multik_201/pass_0 -c ~/workspace/run/overlap_test_multik_201/contigs_10.fasta.gz

			//}
			
			//time ./bin/mdbgAsmMeta countKmer -i ~/workspace/data/AD/shortreads/input_10.txt -c /home/gats/workspace/run/overlap_test_multik_AD/contigs.nodepath.gz.fasta.gz.fasta.gz  -o ~/workspace/run/overlap_test_multik_AD/contig_coverages.tsv
			//!!!Binning: assembly2, permettre de choisir le nom du fichier de contig via -c, actuellement c'est contigs.fasta.gz
			//./bin/mdbgAsmMeta bin -o ~/workspace/run/overlap_test_multik_AD/ -a ~/workspace/run/overlap_test_multik_AD/contig_coverages.tsv

			/*
			//Contig selection
			const string& inputFilenameContig =  _tmpDir + "/tmpInputContig.txt";
			ofstream inputFileContig(inputFilenameContig);
			inputFileContig << _tmpDir + "/tmpContigs.fasta.gz" << endl;
			inputFileContig.close();
			
			command = _filename_exe + " readSelection -i " + inputFilenameContig + " -f " + _tmpDir + "/contig_data.gz" + " -o " + _tmpDir;
			executeCommand(command);
			*/



			prevK = k;
			//getchar();
			pass += 1;
			//if(pass == 2) break;
			//if(k >= 21) break;
			//exit(1);
			//break;
			//cout << "pass done" << endl;
			//if(generatedContigs) getchar();
			//getchar();
			//if(pass >= 2) break;
			//if(k > 30) getchar();

			k += Commons::getMultikStep(k);
		}

		cerr << "Multi-k pass: " << _lastK << "/" << _lastK << endl;
		_logFile << "Multi-k pass: " << _lastK << "/" << _lastK << endl;

		executePass(_lastK, prevK, pass);
		_logFile << "pass done" << endl;

    }

	void executePass(size_t k, size_t prevK, size_t pass){

		bool isFinalPass = k == _lastK;

		writeParameters(_minimizerSize, k, _minimizerDensity, _firstK, prevK, _lastK);

		string command = "";

		if(pass == 0){
			command = _filename_exe + " graph " + _tmpDir + " -t " + to_string(_nbCores);
			if(!_useInitialKminmerFilter){
				command += " --nofilter ";
			}
		}
		else{
			command = _filename_exe + " graph " + _tmpDir + " -t " + to_string(_nbCores);	
		}
		if(pass == 0) command += " --firstpass";
		executeCommand(command);
		//getchar();

		//command = _filename_exe + " multik -o " + _tmpDir + " -t " + to_string(_nbCores);
		//if(!_truthInputFilename.empty()) command += " --itruth " + _truthInputFilename;
		//if(pass > 0) command += " -c " +  _tmpDir + "/contig_data.gz";
		//executeCommand(command);

		//Generate contigs


		//getchar();
		//getchar();


		/*
		command = _filename_exe + " toMinspace " + " -o " + _tmpDir + " -c " + _tmpDir + "/unitigs.nodepath.gz" + " -f " + _tmpDir + "/unitig_data.txt";
		executeCommand(command);
		*/

		//bool generatedContigs = false;
		//if(k == 5 || k == 10 || k == 16 || k == 21 || k == 26 || k == 31){
		if(isFinalPass){

			
			//Generate contigs
			command = _filename_exe + " contig " + " " + _tmpDir + " --final " + " -t " + to_string(_nbCores);
			if(!_truthInputFilename.empty()) command += " --itruth " + _truthInputFilename;
			//if(pass == 0) command += " --firstpass";
			executeCommand(command);

			//getchar();

			command = _filename_exe + " toMinspace " + " " + _tmpDir + " " + _tmpDir + "/contigs.nodepath" + " " + _tmpDir + "/contig_data.txt -t "+ to_string(_nbCores);
			executeCommand(command);

			

			//command = _filename_exe + " toMinspace " + " " + _tmpDir + " " + _tmpDir + "/contigs.nodepath" + " " + _tmpDir + "/contig_data.txt";
			//executeCommand(command);

			cerr << "Removing overlaps and duplication..." << endl;
			_logFile << "Removing overlaps and duplication..." << endl;

			appendSmallContigs();
			
			string contigFilenameCompressed = _tmpDir + "/contigs_H_" + to_string(k) + ".fasta.gz ";
			//getchar();
			command = _filename_exe + " toBasespaceFast " + " " + _tmpDir + " " + _tmpDir + "/contig_data.txt " + " " + contigFilenameCompressed + " " + _inputFilename + " -t " + to_string(_nbCores);
			//if(pass == 0) command += " --firstpass";
			executeCommand(command);



			string contigFilenameCompressed_derep = _tmpDir + "/contigs_H_" + to_string(k) + "_derep.fasta.gz ";
			command = _filename_exe + " derep " + contigFilenameCompressed + " " + contigFilenameCompressed_derep + " " + _tmpDir + " -t " + to_string(_nbCores) + " --nodump";
			executeCommand(command);

			dereplicate();
			

			command = _filename_exe + " circ " + _tmpDir + " -t " + to_string(_nbCores);
			executeCommand(command);

			/*

		
			ofstream contigInputFile(_tmpDir + "/input_contig.txt");
			contigInputFile << contigFilenameCompressed_derep << endl;
			contigInputFile.close();
			command = _filename_exe + " readSelection -i " + _tmpDir + "/input_contig.txt" + " -o " + _tmpDir + " -f " + _tmpDir + "/contig_data.txt" + " -t " + to_string(_nbCores) + " --contig";
			executeCommand(command);
			*/
		
			cerr << "Constructing base-space contigs..." << endl;
			_logFile << "Constructing base-space contigs..." << endl;

			const string contigFilename_uncorrected = _tmpDir + "/contigs_uncorrected.fasta.gz";
			command = _filename_exe + " toBasespace " + " " + _tmpDir + " " + _tmpDir + "/contig_data.txt " + " " + contigFilename_uncorrected + " " + _inputFilename  + " -t " + to_string(_nbCores);
			//if(pass == 0) command += " --firstpass";
			executeCommand(command);
			//getchar();

			cerr << "Polishing contigs..." << endl;
			_logFile << "Polishing contigs..." << endl;
			//./bin/metaMDBG polish ~/workspace/run/overlap_test_201/contigs_uncorrected.fasta.gz ~/workspace/run/overlap_test_201/ ~/workspace/data/overlap_test/genome_201_50x/simulatedReads_0.fastq.gz ~/workspace/data/overlap_test/genome_201_50x/simulatedReads_0.fastq.gz -t 15 --qual
			//getchar();
			command = _filename_exe + " polish " + contigFilename_uncorrected + " " + _tmpDir + " " + Commons::inputFileToFilenames(_inputFilename) + " " + " -t " + to_string(_nbCores) + " -n " + to_string(_contigPolishing_nbReadFragments); //--circ
			executeCommand(command);
			//generatedContigs = true;

			cerr << "Purging strain duplication..." << endl;
			_logFile << "Polishing contigs..." << endl;
			string polishedContigFilename = _tmpDir + "/contigs_polished.fasta.gz";
			string polishedContigFilenamederep = _outputDir + "/contigs.fasta.gz ";
			command = _filename_exe + " derep " + polishedContigFilename + " " + polishedContigFilenamederep + " " + _tmpDir + " -t " + to_string(_nbCores) + " -l " + to_string(_strainPurging_minContigLength) + " -i " + to_string(_strainPurging_minIdentity);
			executeCommand(command);

		}
		else{

			command = _filename_exe + " contig " + " " + _tmpDir + " -t " + to_string(_nbCores);;
			if(!_truthInputFilename.empty()) command += " --itruth " + _truthInputFilename;
			//if(pass == 0) command += " --firstpass";
			executeCommand(command);
			//getchar();
			

			//if(k > _firstK){
			//	const auto copyOptions = fs::copy_options::overwrite_existing;
			//	fs::copy(_tmpDir + "/unitig_data.txt", _tmpDir + "/unitig_data_prev.txt", copyOptions);
			//}

			command = _filename_exe + " toMinspace " + " " + _tmpDir + " " + _tmpDir + "/contigs.nodepath" + " " + _tmpDir + "/unitig_data.txt -t " + to_string(_nbCores);
			executeCommand(command);

			command = _filename_exe + " toMinspace " + " " + _tmpDir + " " + _tmpDir + "/assembly_graph.gfa.unitigs.nodepath" + " " + _tmpDir + "/assembly_graph.gfa.unitigs -t " + to_string(_nbCores);
			executeCommand(command);
			//getchar();
			
			//const string contigFilename_uncorrected = _tmpDir + "/contigs_uncorrected.fasta.gz";
			//command = _filename_exe + " toBasespace " + " " + _tmpDir + " " + _tmpDir + "/assembly_graph.gfa.unitigs " + " " + _tmpDir + "/unitigs.fasta.gz " + _inputFilename  + " -t " + to_string(_nbCores);
			//if(pass == 0) command += " --firstpass";
			//executeCommand(command);
		}	
			
		savePassData(k);
		//cout << "done" << endl;
		//getchar();
	}

	unordered_set<u_int32_t> _duplicatedContigIndex;
	ofstream _outputContigFileDerep;

	void dereplicate(){

		ifstream infile(_tmpDir + "/duplicatedContigs.txt");

		string line;

		while (infile >> line){
			_logFile << "Duplicate: " << line << endl;
			_duplicatedContigIndex.insert(stoull(line));
		}

		infile.close();

		dumpDereplicatedContigs();
	}

	void dumpDereplicatedContigs(){

		string contigFilename = _tmpDir + "/contig_data_derep.txt";
		_outputContigFileDerep = ofstream(contigFilename);

		KminmerParser parser(_tmpDir + "/contig_data.txt", _minimizerSize, _kminmerSize, false, false);
		auto fp = std::bind(&AssemblyPipeline::dumpDereplicatedContigs_read, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		parser.parseSequences(fp);

		_outputContigFileDerep.close();

		_duplicatedContigIndex.clear();
		fs::remove(_tmpDir + "/contig_data.txt");
		fs::rename(_tmpDir + "/contig_data_derep.txt", _tmpDir + "/contig_data.txt");

	}

	void dumpDereplicatedContigs_read(const vector<u_int64_t>& readMinimizers, bool isCircular, u_int64_t readIndex){
		
		_logFile << "contig: " << readIndex << " " << readMinimizers.size() << endl;
		if(readMinimizers.size() == 0) return;
		if(_duplicatedContigIndex.find(readIndex) != _duplicatedContigIndex.end()) return;

		_logFile << "dump contigs: " << readIndex << " " << readMinimizers.size() << endl;
		u_int32_t contigSize = readMinimizers.size();
		_outputContigFileDerep.write((const char*)&contigSize, sizeof(contigSize));
		_outputContigFileDerep.write((const char*)&isCircular, sizeof(isCircular));
		_outputContigFileDerep.write((const char*)&readMinimizers[0], contigSize*sizeof(u_int64_t));

		//_nbContigsPost += 1;
		//cout << "Dump: " << readIndex << " " << readMinimizers.size() << endl;

		//u_int64_t s = 0;
		//for(size_t i=0; i<readMinimizers.size(); i++){
		//	s += (readMinimizers[i]);
		//}

		//_checksum += (s*readMinimizers.size());

	}


	ofstream _fileContigsAppend;
	u_int64_t _nbSmallContigs;

	void appendSmallContigs(){

		_nbSmallContigs = 0;
		_logFile << "Append small contigs" << endl;

		string contigFilename = _tmpDir + "/contig_data.txt";

		_fileContigsAppend = ofstream(contigFilename, std::ios_base::app);

		KminmerParser parser(_tmpDir + "/small_contigs.bin", _minimizerSize, _kminmerSize, false, false);
		auto fp = std::bind(&AssemblyPipeline::appendSmallContigs_read, this, std::placeholders::_1, std::placeholders::_2);
		parser.parseSequences(fp);

		_fileContigsAppend.close();

		//cout << "Nb small contigs: " << _nbSmallContigs << endl;
	}

	void appendSmallContigs_read(const vector<u_int64_t>& readMinimizers, u_int64_t readIndex){
		
		u_int32_t contigSize = readMinimizers.size();
		_fileContigsAppend.write((const char*)&contigSize, sizeof(contigSize));
		
		bool isCircular = false;
		_fileContigsAppend.write((const char*)&isCircular, sizeof(isCircular));
		_fileContigsAppend.write((const char*)&readMinimizers[0], contigSize*sizeof(u_int64_t));

		_nbSmallContigs += 1;
	}

	void savePassData(u_int64_t k){

		const string& dir = _tmpDir + "/pass_k" + to_string(k);

		if(fs::exists(dir)){
			fs::remove_all(dir);
		}

		fs::create_directories(dir);

		//const auto copyOptions = fs::copy_options::overwrite_existing;
		//fs::copy(_tmpDir + "/read_data.txt", dir + "/read_data.txt");


		//if(fs::exists(_tmpDir + "/read_index.txt")) fs::copy(_tmpDir + "/read_index.txt", dir + "/read_index.txt");
		//if(fs::exists(_tmpDir + "/minimizer_graph.gfa.gfa")) fs::copy(_tmpDir + "/minimizer_graph.gfa.gfa", dir + "/minimizer_graph.gfa.gfa");
		//if(fs::exists(_tmpDir + "/minimizer_graph_u.gfa")) fs::copy(_tmpDir + "/minimizer_graph_u.gfa", dir + "/minimizer_graph_u.gfa");
		if(fs::exists(_tmpDir + "/assembly_graph.gfa")) fs::copy(_tmpDir + "/assembly_graph.gfa", dir + "/assembly_graph.gfa");
		if(fs::exists(_tmpDir + "/assembly_graph.gfa.unitigs")) fs::copy(_tmpDir + "/assembly_graph.gfa.unitigs", dir + "/assembly_graph.gfa.unitigs");
		//if(fs::exists(_tmpDir + "/assembly_graph.gfa.unitigs.index")) fs::copy(_tmpDir + "/assembly_graph.gfa.unitigs.index", dir + "/assembly_graph.gfa.unitigs.index");
		//fs::copy(_tmpDir + "/contigs_path.csv", dir + "/contigs_path.csv");
		//fs::copy(_tmpDir + "/minimizer_graph_cleaned.gfa", dir + "/minimizer_graph_cleaned.gfa");
		fs::copy(_tmpDir + "/parameters.gz", dir + "/parameters.gz");

		//if(k == _firstK || k == _lastK){
			//fs::copy(_tmpDir + "/minimizer_graph.gfa", dir + "/minimizer_graph.gfa");
			//fs::copy(_tmpDir + "/kminmerData_min.txt", dir + "/kminmerData_min.txt");
			//if(fs::exists(_tmpDir + "/nodeName_to_unitigIndex.bin")) fs::copy(_tmpDir + "/nodeName_to_unitigIndex.bin", dir + "/nodeName_to_unitigIndex.bin");
			//if(fs::exists(_tmpDir + "/groundtruth_position.csv")) fs::copy(_tmpDir + "/groundtruth_position.csv", dir + "/groundtruth_position.csv");
			//if(fs::exists(_tmpDir + "/read_path.txt")) fs::copy(_tmpDir + "/read_path.txt", dir + "/read_path.txt");
			//if(fs::exists(_tmpDir + "/read_path_cleaned.txt")) fs::copy(_tmpDir + "/read_path_cleaned.txt", dir + "/read_path_cleaned.txt");
		//}
		//fs::copy(_tmpDir + "/mdbg_nodes_init.gz", dir + "/mdbg_nodes_init.gz", copyOptions);

		//fs::copy(_tmpDir + "/unitig_data.txt", dir + "/unitig_data.txt"); //Debug
	}



	void writeParameters(size_t minimizerSize, size_t k, float density, size_t firstK, size_t prevK, size_t lastK){


		float minimizerSpacingMean = 1 / density;
		float kminmerLengthMean = minimizerSpacingMean * (k-1);
		float kminmerOverlapMean = kminmerLengthMean - minimizerSpacingMean;

		string filename_parameters = _tmpDir + "/parameters.gz";
		gzFile file_parameters = gzopen(filename_parameters.c_str(),"wb");
		gzwrite(file_parameters, (const char*)&minimizerSize, sizeof(minimizerSize));
		gzwrite(file_parameters, (const char*)&k, sizeof(k));
		gzwrite(file_parameters, (const char*)&density, sizeof(density));
		gzwrite(file_parameters, (const char*)&firstK, sizeof(firstK));
		gzwrite(file_parameters, (const char*)&minimizerSpacingMean, sizeof(minimizerSpacingMean));
		gzwrite(file_parameters, (const char*)&kminmerLengthMean, sizeof(kminmerLengthMean));
		gzwrite(file_parameters, (const char*)&kminmerOverlapMean, sizeof(kminmerOverlapMean));
		gzwrite(file_parameters, (const char*)&prevK, sizeof(prevK));
		gzwrite(file_parameters, (const char*)&lastK, sizeof(lastK));
		gzwrite(file_parameters, (const char*)&_meanReadLength, sizeof(_meanReadLength));
		gzclose(file_parameters);
	}


	/*
	void createInputFile(bool useContigs){
		_inputFilenameComplete = _tmpDir + "/input.txt";
		ofstream inputFile(_inputFilenameComplete);

		ReadParser readParser(_inputFilename, false);

		for(const string& filename : readParser._filenames){
			inputFile << filename << endl;
		}

		if(useContigs){
			inputFile << _tmpDir + "/tmpContigs.fasta.gz"  << endl;
		}

		inputFile.close();
	}

	string createInputFile(u_int32_t k){
		string inputFilename = _tmpDir + "/input.txt";
		ofstream inputFile(inputFilename);

		//const string& filename = _tmpDir + "/correctedReads_" + to_string(k) + ".min.gz.bitset";
		const string& filename = _tmpDir + "/read_data.txt.corrected.txt";
		inputFile << filename << endl;


		inputFile.close();

		return inputFilename;
	}
	*/

	/*
	vector<u_int64_t> _readLengths;
	u_int64_t _readLengthSum;
	u_int64_t _readLengthN;

	u_int64_t computeMeanReadLength(const string& filename){




		_readLengthSum = 0;
		_readLengthN = 0;

		auto fp = std::bind(&AssemblyPipeline::computeMeanReadLength_read, this, std::placeholders::_1);
		ReadParser readParser(filename, false, false, _logFile);
		readParser._maxReads = 100000;
		readParser.parse(fp);

		if(_readLengthN == 0){
			cerr << "No reads provided (check input filenames)" << endl;
			exit(1);
		}

		return _readLengthSum / _readLengthN;
	}
	
	void computeMeanReadLength_read(const Read& read){
		_readLengthSum += read._seq.size();
		_readLengthN += 1;
		_readLengths.push_back(read._seq.size());
	}
	*/

	void executeCommand(const string& command){

		//cout << command << endl;
		//string command2 = "time -v \"" + command + "\" 2>&1 " + _tmpDir + "/time.txt";

		Utils::executeCommand(command, _tmpDir, _logFile);
		//getchar();
	}

	void checkDependencies(){

		string read = "";
		for(size_t i=0; i<10000; i++){
			read += "A";
		}
		read += "\n";

		//string read = "TTGCACTCATGAAGGTTTCATGGTATTCTAAAGAAATAAAATCAAAAATTAAATCCAAAAATCAGGCAATGAAGTTCTAAGCAAACTTTCACTTACACCCTGTACATTCGCTTAAATGAAGCCATATAAGGCTAGGTGGGGTGTGAAGTTGAAAATATCTATGCGCTAAAGGGCTAAAATCATTTGGTAAACCTAAAATTTTAATAATAGAATTTACAGCCTCTAATGAGATATTTTTTTCTTCTTTGTGTGAGTTTTCCAATAGAGTGATTTTGCGCTCTAATATGCTTTTTAATTCTTCATTGCTTAAAGACATTTTTTAAACCTTTAGCTATTTGTATCATATTTAAAAAATCATTTGCATTTAATGCAGCCGAACCTATTAATACTCCATCACAATTTTTTAAAGTACAAATTTCTTTAATATTGTTTTGATTAACACTTCCACCATATAAAAGCTTAGCGTTAGTAAATTCTCTTAAAAAATTAAGTATAGTGTTAATATCATCAAGATTAGCACTTACTCCAGTGCCTATAGAATAAATAGGTTCATAAGCTATGATAAGTTTTTTATAAGATAAGTCAATATTTTCAATTTGTTTTTTTAAAAAGTCTAAGCTTTTGTTAGCATTTTTAGTTTCTAAACTTTCGCCTATACAATAAATGATATTAAAATCAAGATTTTTTGCAAAGTCAAATTTGGTCTTTAAAAAACTTTCATCTTCACTTAAAGCTCTTCTTTCAGAATGGCCTATCAAAACGCTTTTGATATTAAAATTCTTCTAAGTGAATTTTACCTATTTCTCCAGTATACGCTCCATTTTCACAAGGGTAGAAATTCTGCGCGCCTTGTTGAAAGGAAAAATTGTCTTTTAAAAAAGCTATACTAGGAGGGAAGATGAAAACTTCATCTTCTTTGTTTAAGTTTTGGTTTAATTTTTGAGCGTAAAGTTCAAAGCTTGATCTTGTATGATTACACTTTAAATTTGCTGCAAAAATCATTCATTATCCTTTATCGTTAAAGGTTTTACACCAGGGAAGTTCTTTTCCTTCTATAAAGCTCTAATGATGCTCCACCACCGGTTGAAATGAAAGTCATCTCATCAGCATCACCTGCTCTTGCTACAACATCAGCAGTATCACCACCACCTATTACAGTTGTCCGCATGAAGATTCGCTAATATAATGGCTCATTTTAATGCTGCCTTTTGAGAATTTATCGATTTCAAAAACCCCCATAGGTCCATTCCACCATATAGTTTGAGCATCAGAAAAGTGCTTCTTTAAACAACCTTACACTAGCAGGACCTATATCAAGTCCCATCCAACCTGCAGGAATTTCTTGCACAGGGGTATATTTCATTACTGCTTCTTGAGAGCAAGTTTGTGCTGCTGTAACATCCACAGGAAGATAAATTTTTACACCTAGATTTTTACCTTTGAGTAAGATTTTATTTGCTTCTTCAATTAAATCTTCTTCTAAGAGAGAATTTTCCTATATCATAACCTTGAGCTTTTAAGAAAGTAAAGGCCATACCTCCACCTAGATGAGCTTATCTACTTTTGGAAGTAAGTTGGTCAAAGCTTGTAGTTTTCCGCTTACTTTAGAACCACCTACTACCGCCACAAAAGGTCGTGCAGGGTGTTGATGAGATTACTTGCAAATTCTATTTCTTTTTGGAGTAAAAATCCCGCACCTTTGTTTGTATTGTCAAAGTATTTAGTGATAGCTTCAACACTAGCATGGGCTCTATGACAAACCCCAAAGGCATCATTAAATAAACATCAGCCATAGAAGCAAGTTCTTTAGCTAAGTTTTCATCATTTTTAGTTTCACCCTTTTCAAAGCGTAAATTTTCTAAAAGTAAGATTTCACTCGGCTTTAATTCATTAGCTTTTTTCTTTGCATCTTCACCTATAACATCTTTAGCCATAATGACTTCTTTAGCCATTAAACGTGCAAGCCTTTTTGCAACCGGTTCTAAAGAGTATTTTGAGGCTATTTCTTTTGGACGTCCTAAATGTGAAGCCAAAATAACCGCACAACCATTGTCTAAACAATATCTTATAGTAGGGATAGCCGAGCGAATACGACGATCATCAGTAATATTTAAAAACTCATCTTGAGGAACATTAAAATCACACCTAATAAATACTTTTTTCTTTGCAAGATCGATATCTTTAATAGATAAAATACTACTCATCTTAAGCCTTTGCTACAAATACTGCCATATCTACTAAACGAGAAGAATATCCCCATTCATTATCATACCAAGCAACCACTTTAACAAAATCATCGCAAATTACTTGAGTTAAATCACTTGCTACAATTGCACCATAAGAGCAAGTTATAAAATCACTTGAAACTCTTTCTTCATCATCAACCAATAGTATGCCTTTTAAATTGCTAGCAGCTGCTTTTCTAAAGGCTTCGTTAATTTCTTCTTTACTTACTTTTTTCTTTAAAGTTGCAGTTAAATCCACAGTTGATACATCAGCCACAGGTACACGCATGCTTTGACCATGTAATTTTCCAATCAAGTTCAGGCATAACAAGTTTCATAGCTTTTGCTGCACCAGTTGAAGTAGGGATGATATTTTGAGCAGCAGCACGTGATCTTCTTTTATCTCTAGCCTTTCATCAATAATGCTTTTCCGTTTGTTGTAGGCATGAATAGTTGTCATTAAACCTTTTTCTATGCCGAAATTATCTTGTAAAACTCTACAAATTGGACCTAGACAGTTAGTAGTACAACTTGCATTTGAGATGATGTTTTCGCCTTTGTATTCGTGAGCATTTACCCCTAAAACATAAGTTTGGAGTTTTATCTTTTGCAGGTGCACTCATGATAACTTTTTGCACCCCATGATCTAAAAATCCTTGACATTTTTCCATAGTTAAAATGTGCGCCAGTACATTCTAAAACAATTTGTACGCCGTATTTTGCAAAATCTAAATCAGCTACATTTCTACTTTTTAAAACTTTTATTTTTTTATTATCAATTATTAAATCATCACCTTCATTTTCAACACTACCATCATATACCCCATGAACAGTGTCATATTTAAAAAGATATTTAGTAAGCTCTATATCAGTAGTATCATTAATCGCTACAAGCTCGATATCATCGCGCTTCATAATAATTCTTGCAACACATCTTCCAATGCGTCCAAAACCATTTATTGCAACTTTTACAGCCATTTTCATTTCCTTTTTATGATTAAGTAGGTAAAATTCTACTAAAAAAAGGTTAAATTTTAATGAAAAATCGCACTTTTTGGGTGGCAGTTTTGATCCACCCCATTTAGGGCATAATGCTATAGTCTTTAATGCGCTAGCAAATTTAGAGCTTGATAAGCTTATTATTATGCCAACTTTTATTAGCCCCTTTAAGAAGAAATTTACCGCAAATGAACAAAGACGCTTAAAATGGTGTGAGATGATTTGGGGAGGTTTGGAAAAAGTTGAAATTTGTGATTTTGAAATAAAAAAACAAAGACCCGTACCTAGTATAGAAAGTGTTGAATTTTTTGTACAAACAATATGAAATTTCTAAATTTTTATCTTATTTTGGGAGCTGATCATTTACAAAGTCTTGAAAAATGGCATGAATTTGAAAGATTGCAAAATTTTGGTAGAATTTGTTGTAGCTAAAAGAGATGATTATTTTTATACCAAAACATTTTAAAACCTTAGATACTAAAGTAGATATCTCTTCTTCTTTTATAAGAGAAACCTTGCAAACATCACAAGTTTGGTGAGCAAATCAAAGAAGAAGTTAGCTTTATTATTCTAAATTTAAAAATATTTAACAAAGGAAAATAAATGCAAGAAAGAATTAATAATATAGGCAAATTTTAGATGATAAAAAAGCAGACTTAATAGAAACTTTTGATATGCAAGATAAGGATTATTTTGTCAAATTGTAGTTGATTGCTACCAACTATGGGGGAAAGACATGCGCTTTCTTTGATTGATGATTTAAAAACTAATCTTAAAAGTAAGGTGAAGAATTTTTAAATATAGAAAGTAGTGAAGAATGGACTGTACTTGATTTGGGAGATATTTTGATTCACTTAATGAGTGAAACTTATAGGGCAAAATATAATATAGAAGAATTTTTAAAAAGTTTAAACAAAGAAAATCAAAACTAGGAAATAAAATTGGATAATTTAATTACTGAGTTATCAGATTTAGCTAAAGAATATTTTGAAAATTCAGAATTTGAAAAATGTGATGAAGTCTTAAGTGAGCTTATAGCTTTTTGTAAGGGTGAAGTTACGCTTGCTCAAGATGGTAAAATGATTTTTATAGATGATGAAGATAAACACCGTTTGCTTTTAGAGGCTTATCATATAGAGCGTTTTGTAGATTTAATTGCTTGAAATTTCAAGAAGCGCTTGAAGATGCAACTATAGCTGTAGAATTTGATTCTAGTAATGTATTTTTGTTTAATCTTTTGGGCTTGTGTAATTTTAAGTTAGATTTATTACAAGAAGCACTTTTAGCTTTTAATAAAACTATAGCACTGGATAATACATATTATTTAGCATATTTTAACAGAGCTAGGTTTATATACTTTTAGATGAGCAAACAAAAGCTTTAAAAGATTTACAAACTTGTATCGATTTAGCTCCAGAATATTATATAGCTTATTATACTAGAGCCATTGCCCTGCTTAGTGTTGATCCAAAACAAGCTTTGCTTATTTTAAGAAGTCTCAAGATTTGGGTTTTGAATCTTCGCAGATTTTTTATTATCAAGGGGTAGCTTATGAGAATTTAGAAGATTATCAAAAAGCTATAGAATTTTTTACCAAGATGATTGAGCAAGATGAGAGTTTTTCTGATGGTATTATAAAAGGGCTAATAATAACGTAAAGTCGATGATTTAGAGGGCGCTTTGCAAGATTGCTTAAAATCCATTGAATTAGATAATGAAAATGCAATGGCATATTTGATTTTGGGACATATTTATAAAGACTTAGAAAAGATAGAATTTATCAATAGATGCTTTTAAAAAATCAATCGAATTGGATGAAAATTTGCAATATCCATGTTTTGCATTGGCTAGGGTTTTGTATGATTTAGAAGATTATGAGCAAGCTTTGAAGTATTATGATAAAACAATTGAGCTTTATGAAGAATATGCCCAAGCATATATTAATAGAGGCAATACTAAAATCAATCTAAAAATGATTGAAAGAGGCTATAGAGGATTTTGATTTAGCAGCTAAATATTATCAAGAAAGAGCAAGTAAACCATGATTTTACTCCATCAGAATTAGCAGAACTTGAACACGCAGTTCCTTATGGTTTTTATCATTTGGGCAATAGCTTATACGAGATAGATAAATATGATGAAGCTTTGATAAGCTATGAAAAAGCTTTAAAATATCAAAAAGAATATCCCGATGTATTTTTTAATAGGGCCTTATTTAAAATCTGATCTTGAAAAATATGAGGAAGCTTTAGAAGATAGTGAACTAGCTGTTAAATATTATAAAAAAGCAAAATAATACCCCAAGATTATGCTAATTCCCTTTCTCAAAGAGCTTGGATTAAAAGTAGACTTGAAAGATTTCAAGAAGCTATGGATGAATACAATGAGCTTATAAAAACATATAAAGATTATATTGATTTAAAAGATGTGTTATTTGAAAGAGCATATTGTGCAAAAGAACTTGAATCATATGAAGAATTAATAGCTTATTGTAATGCAGCACACAAGGTGGATAAAAATAATTTTAAACTTTATTTTTGGCGAGGAATTGCTAAATATAATTTAAGCTTAGAAGAAGAAGCTATAGAAGACTTAAACAAGGCTTTAAAAATCGATAAAAACCATAAATGGTGCAAAATATTATAAAGGACTTTGTTTGAAGATCTTTATATGTTTGAAGAGGCTATAAAATGTTATGATAGCGTGATTTTAAGTAACGAAGAGGATGATGAGTCGTATTTTCATAGAGCAAAATGCAAAAGAAATTTGAAAAATATAACGAACTTTAAAAGATATCAATGAATGCTTAAAAATAATTGATGATATTGGGGAGTATTGGATAGAAAAAGCACAAATTTTAAGTTTTTTAGGAAAATATGATGAAAGCTTTGAAGCGGCAAAAAAGGCAAGTGAGCTTGAGCCAAAATCATATGAGTGTTATCATTTTATGGGTGCAGTTAAAGTGTATTCGCAAGATTTTAAAGAAAGCTATAAAATATCTTAACATGGCTTTAAATTTTAGATGATAGTCAAAATTGGACACATTATTACAAAGGCAGAGTGTTTAAGAAATTTAGGTGATTTTCATGATGCATTGCAATGTTATGAAGATTGTTTAAAGATAGTAAATGAAAATACCGATGCTTTGGTGGGAAAAATTCAGTGTTTAGAAGAGTTAAAAGAATACGAACAAGCTCTAGAGTGTACTAAAGATTTATTAAAACTTGATGAAGAAAATGAGTTTGCATTAAAAAATCAAGATATTTAATGCAAAAATTAAAACAGCAAAATAAAAAATGGTGGCAAATTTGGTAGCGAAAAGAATAAATTTTTAACACTTTGGTAATTTCTAAAATATTTTTCTAAATATTTTATACGTTTTTCAAGGCTTGGGTGTGTGGCAAAATACCATCGAAAGCTTTTAAGAAAAAAATATGTGCACAATCTTTTGCTTTTATATTTGTCGATATTTGTTTTCTTTTGAAATAATATTAATTTTTTTATAGCATTGATTAGAGCTTGTGGATTTCTATATATTGCACACTTGAAACATCAGCTAAATATTCTTTTTGGCGTGAAATACATGCTTGTAGTATTCTAGCAAATAAAGTTCCCAAAGCACCTAAAATAAAAAACACTAAAGCTATTGCTCCTAAAACAGCTAAAGCTTTATTGTGATCCTTTGAGTTGCTTGTTTTTGTTTTGCTTAAACTTCTTAACACTATATCTCCTATTAAAGAAATGCCAATTTAATGCAAAAATTAACGAAAAAAGTTTTAAATTTAGCACCACATCTTTATGAAAAGCATGAGAAAATTCATGACCGATGACCGCTTGCAGTTCTTCTTTATGAAAAAATCCAATGCACCTTTTGTGATAAAAATTGCTATTTTTTCATCTTTTTTGCCAAATTTTTCACCTGTACAAAGTGCATTAATTCCTTTTTCTTTAAACATAATGAAAATTCTAGGAAGCGGCATATTTGAAGCTATAGCCATTTCTTCTATAGCAAAAAGTGCTTTTTTTTCTTTAGCGGTAAGATTTTTTCTACTAGGTGAAATTTCTTTTGCTTTTAAAATTTTGGCAAGTTTATCAATGCTTTTATTTTTCATTATAAAAACCTTGATCAAATAAGCAAAGAAAATTATCATAAGTACAATAAAGCTAAGTAGAAAAGATATTATTTTATAGACTTCATAATTTCTATTGTCAAAAAACTCAACACCGTGATAGGGTATAAGGTGTGTGATATGAAAAAATCTCACGACTTTCATAAAAATCAGGTGTAAACATATAAAAAACTTTTAAAATCAAAGAATAGGAAAGTAAAACGATGAAAAAAAGTAAAGCAAAAAATAATAAATTTAGCCACAAAATCCTAAATTTAGAAATATTTTGAAGTCATAAATATCATTTTTTAATTTAGCTTGTTTACCTAAGATAAAATGAGTTATGATCAAAATTACTAAAAAACTTATAAAAACAAAGCAAAGACTTATAAGTAAGTTATTTGCTATAAAATAAAATATATCAATTAAAATTGATTTTGCCTTAATTCTCAAAAACTAATAATAAATATTTTAAAATAGGTAGTATTTTTTCTATGATTAATATCCGCTGATTGCTCAAGTACCATTATAATTTAAACAAAACAAATATAATAAGAAGTACGGATAAAAATATTAATTATTTGTTTTAGCAATTTTTATCCTTTAAAATTTACTTTTGGCGCTTTAGTATTTCTTGTTTATCTTCAAATTCAAGCATTCTAAATTTAGGAAAATATCGCACGACTAAATTTGCTGGAAAACTTTCTCTAGCGGTATTATAATGCATAACACTATCATTGTAAGCTTGTCTTGCAAAAGCGACTTTATTTTCAGTGTGAGTAAGTTCCTCACTTAATTGTGTTAAATTTTCATTAGCTTTTAAATCAGGATAATTTTCCAAAAACCATATTGAAATTTGAAAGTGCAGAATTTAACGAATTTGTGCTTCGCTTAAATCTTGCAAACTTTTGACATTTGTTAAATCAGCATTTCGACTTACATTTAAGGCGTTGTTTCTTGCTATAGTAACTTTTACAAAGTGTATTTTCTTCATATGAAACATATTTTTTAGCTATTTCAACAAGATTTGGGATTAGATCATATCTTCTTTTCAGTTGTGCATCAATTTATGAAAAGGCATTATAAGCACGATTAAATAAAGTTACAAGATTGTTATAAATACTTACAATATAGATAATCAAAACTAATATAATTACTAAAAATATCACAGAAATTATCATTTCATATCCTTAAAATAAATTATTTTTGCTCCATTTAGGCAGATTTTTCTCTAAACCAGAAGAATGAAACATAAAAAATTAATCATTTCTTCAAAATCTTTTACTTTCCAATTATCTATATTTTGCCTAAAAGAACTAGCATTAGCAAAAATAAAGATCCATTTTTACAACATTACTCACATCCCATTTGTCCAAAGGCTGATTAAAGCTTGAAGCTTTTATTAAACATCATTCTCATGTCTCTTACCTTGGAAACATCCCAAGAATTAATGTTTTGATTAAAAGAACTAGCATTTGCAAACATTGCGTCCATTTTTATAACATTGCTTGTATCCCATTTGTCAAGTGCTTGGTTAAATTTCAAAGCTCCTGTAAACATTCTTGACATAATTTTGACACTACTTACATTCCAGTCATTTAAAGGTTGGTTAAAAGAAATAGATTCTGAAAACATGCCCGCCATATTTTCAACTTTACTTACATTCCAAGAGCTAATATTTTCATTGAAGTTTTTTGTTTGTGCAAACATATTGCTCATATCAATAACATTGCTTGTGTCCCATTTATCAATCCCACTAAAATCTTTTCTAGTAGCTCGTAAAAATAAAGATTTCATATTTGTGATTTTGCTTGTATTGATATCACC";
		string filename = _tmpDir + "/testDependencies.fasta";
		string outputFilename_mapping = _tmpDir + "/testDependencies_align.paf";
		string filenameBzip = _tmpDir + "/testDependencies_bgzip.fasta.gz";
		string outputFilename_mapping_minimap = _tmpDir + "/testDependencies_minimap.paf";

		if(fs::exists(filename)) fs::remove(filename);
		if(fs::exists(filenameBzip)) fs::remove(filenameBzip);
		if(fs::exists(filenameBzip + ".fai")) fs::remove(filenameBzip + ".fai");
		if(fs::exists(outputFilename_mapping)) fs::remove(outputFilename_mapping);

		ofstream file(filename);

		file << ">read1" << endl;
		file << read << endl;
		file << ">read2" << endl;
		file << read << endl;
		
		file.close();


		cerr << "Checking dependencies: bgzip (wfmash package) ";
		_logFile << "Checking dependencies: bgzip (wfmash package) ";
		string command = "cat " + filename + " | bgzip --threads " + to_string(_nbCores) + " -c > " + filenameBzip;
		Utils::executeCommand(command, _tmpDir, _logFile);

		if(!fs::exists(filenameBzip)){
			cerr << endl << "Dependency is missing: bgzip (wfmash)" << endl;
			exit(1);
		}
		else{
			cerr << "ok" << endl;
		}

		cerr << "Checking dependencies: samtools ";
		_logFile << "Checking dependencies: samtools ";
		command = "samtools faidx " + filenameBzip;
		Utils::executeCommand(command, _tmpDir, _logFile);

		if(!fs::exists(filenameBzip + ".fai")){
			cerr << endl << "Dependency is missing: samtools" << endl;
			exit(1);
		}
		else{
			cerr << "ok" << endl;
		}

		cerr << "Checking dependencies: wfmash ";
		_logFile << "Checking dependencies: wfmash ";
		command = "wfmash " + filenameBzip + " -t " + to_string(_nbCores) + " > " + outputFilename_mapping; //-l 5000 -p 80
		Utils::executeCommand(command, _tmpDir, _logFile);

		if(!fs::exists(outputFilename_mapping)){
			cerr << endl << "Dependency is missing: wfmash" << endl;
			exit(1);
		}
		else{
			cerr << "ok" << endl;
		}

		cerr << "Checking dependencies: minimap2 ";
		_logFile << "Checking dependencies: minimap2 ";
		command = "minimap2 -x map-hifi " + filename + " " + filename + " > " + outputFilename_mapping_minimap;
		Utils::executeCommand(command, _tmpDir, _logFile);

		if(!fs::exists(outputFilename_mapping_minimap)){
			cerr << endl << "Dependency is missing: minimap2.24+" << endl;
			exit(1);
		}
		else{
			cerr << "ok" << endl;
		}

		cerr << endl;
	}

};	



#endif 



