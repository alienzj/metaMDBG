

//#include "Graph.hpp"
//TODO:
// remove bubble, tips: consider length

//#define PRINT_DEBUG_SIMPLIFICATION



#ifndef MDBG_METAG_GRAPHSIMPLIFY
#define MDBG_METAG_GRAPHSIMPLIFY

#include "Commons.hpp"
#include "ProgressiveAbundanceFilter.hpp"
#include <limits>
//#include "contigFeatures/ContigFeature.hpp"
//#include <experimental/filesystem>

//using namespace std;
//namespace fs = std::experimental::filesystem;

//class GraphSimplify;

struct NodeAbundance{
    u_int32_t _nodeIndex;
    double _abundance;
};

struct UnitigTip{
    u_int32_t _unitigIndex;
    u_int32_t _startNodeIndex;
    u_int32_t _length;
};

struct BubbleSide{
    u_int32_t _unitigIndex;
    u_int32_t _nbNodes;
    u_int32_t _nodeIndexSum;
    //u_int32_t _quality;
};

struct Bubble{
    u_int32_t _startNode;
    u_int32_t _endNode;
    vector<NodeAbundance> _nodes;
};


//const float _globalCutoff = 3;

struct Unitig{
    u_int32_t _index;
    u_int32_t _startNode;
    u_int32_t _endNode;
    float _abundance;
    u_int32_t _length;
    u_int32_t _nbNodes;
    vector<u_int32_t> _nodes;
    //u_int32_t _quality;
    //u_int32_t _debug_nodeIndexSource;
    //u_int32_t _debug_nodeIndexSum;
};

struct ComplexArea{
    u_int32_t _nodeIndex_source;
    u_int32_t _nodeIndex_sink;
    vector<u_int32_t> _unitigs;
    vector<u_int32_t> _path;
    //float _abundanceCutoff_min;
};


//"todo: take superbubble dans algo d'assemblage, ça peut loop actuellement"
//"calculer la vrai length des unitigs en prenant en compte les overlaps"
//"ne plus utiliser de component connexe qunad abondance < 30"

struct SaveState{
    //float abundanceCutoff_min;
    unordered_set<u_int32_t> _isNodeValid2;
    vector<bool> _isBubble;
    unordered_set<DbgEdge, hash_pair> _isEdgeRemoved;
    vector<Bubble> _bubbles;
    unordered_map<u_int32_t, u_int32_t> _nodeToUnitig;
    vector<Unitig> _unitigs;
    unordered_set<u_int32_t> _isLongUnitig;
    unordered_map<u_int32_t, u_int32_t> _longUnitigNodeAbundance;
};

struct SaveState2{
    //float abundanceCutoff_min;
    float _abundanceCutoff_min;
    unordered_set<u_int32_t> _nodeNameRemoved_tmp;
    vector<u_int32_t> _nodeNameRemoved;
    vector<DbgEdge> _isEdgeRemoved;
    vector<u_int32_t> _isBubble;
    vector<NodeAbundance> _longUnitigNodeAbundance;
    vector<u_int64_t> _rareReads;
    //vector<Bubble> _bubbles;
    //unordered_map<u_int32_t, u_int32_t> _longUnitigNodeAbundance;
};

class GraphSimplify{

public:


    ofstream& _logFile;
    BiGraph* _graphSuccessors;
    //BiGraph* _graphPredecessors;
    string _outputDir;
    string _inputGfaFilename;
    vector<Unitig> _unitigs;
    //vector<u_int32_t> _nodeToUnitig;
    vector<u_int32_t> _nodeToUnitig;
    //vector<u_int32_t> _nodeAbundances;
    //vector<u_int32_t> _nodeLengths;
    //unordered_map<Destroy path: 2u_int32_t, u_int32_t> _nodeAbundances;
    //unordered_map<u_int32_t, u_int32_t> _nodeLengths;
    //vector<bool> _isNodeValid;
    unordered_set<u_int32_t> _isNodeErased;
    //unordered_set<u_int32_t> _isNodeValid2;
    //unordered_set<u_int32_t> _isNodeValid2_cache;
    vector<bool> _isBubble;
    //unordered_set<DbgEdge, hash_pair> _isEdgeRemoved;
    //unordered_set<DbgEdge, hash_pair> _isEdgeUnsupported;

    u_int32_t _nbNodes;
    vector<Bubble> _bubbles;
    unordered_set<u_int32_t> _isNodeInvalid_tmp;
    unordered_set<u_int32_t> _isUnitigInvalid_tmp;
    //GraphSimplify::SuperbubbleSimplify* _superbubbleSimplify;
    //u_int32_t _lastAbundanceCutoff;
    unordered_map<u_int32_t, ComplexArea> _complexAreas_source; 
    unordered_map<u_int32_t, ComplexArea> _complexAreas_sink;
    //unordered_set<u_int32_t> _isLongUnitig;
    unordered_map<u_int32_t, u_int32_t> _longUnitigNodeAbundance;
    vector<vector<UnitigLength>> _startingUnitigs;
    vector<Unitig> _startingUnitigstest;
    unordered_set<u_int32_t> _currentUnitigNodes;
    unordered_set<u_int64_t> _removedReadIndex;
    unordered_set<u_int32_t> _isBoundUnitig;
    unordered_set<u_int32_t> _debug_groundTruthNodeNames;
	vector<UnitigData> _unitigDatas2;
    unordered_set<u_int64_t> _rareReads;
    unordered_set<u_int32_t> _repeatedNodenames;
    size_t _kminmerSize;
    unordered_set<u_int32_t> _isNodenameRoundabout;
    bool _isCopy;
    vector<Unitig> _cleanedLongTips;
    //ContigFeature* _contigFeature;
    int _nbCores;
    u_int64_t _superbubble_checksum;
    u_int64_t _tip_checksum;
    u_int64_t _abcutoff_checksum;
    vector<bool> _isNodeRemoved;

	//float _minimizerSpacingMean;
	float _kminmerLengthMean;
	float _kminmerOverlapMean;

    UnitigGraph* _unitigGraph;

    GraphSimplify(const string& inputGfaFilename, const string& outputDir, u_int32_t nbNodes, size_t kminmerSize, int nbCores, float kminmerLengthMean, float kminmerOverlapMean, ofstream& logFile) : _logFile(logFile){

        _unitigGraph = nullptr;
        _kminmerLengthMean = kminmerLengthMean;
        _kminmerOverlapMean = kminmerOverlapMean;

        _superbubble_checksum = 0;
        _tip_checksum = 0;
        _abcutoff_checksum = 0;
        //_bubble_checksum = 0;

        //_contigFeature = nullptr;
        _inputGfaFilename = inputGfaFilename;
        _outputDir = outputDir;
        _kminmerSize = kminmerSize;
        _nbCores = nbCores;

        _graphSuccessors = GfaParser::createBiGraph_binary(inputGfaFilename, true, nbNodes, kminmerLengthMean, kminmerOverlapMean);
        _nbNodes = _graphSuccessors->_nbNodes;
        _isCopy = false;

        //_logFile << "lala " << _graphSuccessors->_nodeAbundances.size() << " " << _graphSuccessors->_nodeLengths.size() << endl;
	    //_graphPredecessors = GfaParser::createBiGraph_lol(inputGfaFilename, false);
        //clear(0);
        //_isNodeValid = vector<bool>(_graphSuccessors->_nbNodes, true);
        //_isBubble = vector<bool>(_graphSuccessors->_nbNodes, false);

        //_superbubbleSimplify = new SuperbubbleSimplify(this);

    }

    /*
    GraphSimplify(GraphSimplify* graph, const unordered_set<u_int32_t>& validNodeNames){

         _isCopy = true;
        _inputGfaFilename = "";
        _outputDir = "";

        _graphSuccessors = graph->_graphSuccessors;
        //_nodeAbundances = graph->_nodeAbundances;
        //_nodeLengths = graph->_nodeLengths;


        //_logFile << _graphSuccessors->_nodeAbundances.size() << " " << _graphSuccessors->_nodeLengths.size() << endl;
        //_logFile << _nodeAbundances.size() << " " << _nodeLengths.size() << endl;
        //getchar();

        //for(u_int32_t validNodeName : validNodeNames){
        //    _isNodeValid2.insert(BiGraph::nodeName_to_nodeIndex(validNodeName, true));
        //    _isNodeValid2.insert(BiGraph::nodeName_to_nodeIndex(validNodeName, false));
        //}

        //_isBubble = vector<bool>(_graphSuccessors->_nbNodes, false);
        //_isEdgeRemoved.clear();
        _bubbles.clear();

        //for(size_t nodeIndex=0; nodeIndex<_graphSuccessors->_nbNodes; nodeIndex++){
        //    _isNodeValid2.insert(nodeIndex);
        //}

        //_nodeAbundances = vector<u_int32_t>(_graphSuccessors->_nbNodes/2, 10000);
        //_nodeLengths = vector<u_int32_t>(_graphSuccessors->_nbNodes/2, 10);
        


        //_logFile << "getSuccessors 564 debug to remove" << endl;


	    //_graphPredecessors = GfaParser::createBiGraph_lol(inputGfaFilename, false);
        //_nodeAbundances.resize(_graphSuccessors->_nbNodes/2, false);
        //_nodeLengths.resize(_graphSuccessors->_nbNodes/2, false);
        //GfaParser::getNodeData(inputGfaFilename, _nodeAbundances, _nodeLengths);
        //_isNodeValid = vector<bool>(_graphSuccessors->_nbNodes, true);
        //_isBubble = vector<bool>(_graphSuccessors->_nbNodes, false);

    }
    */

    ~GraphSimplify(){
        if(!_isCopy) delete _graphSuccessors;
        //delete _graphPredecessors;
    }

	static bool UnitigComparator_ByIndex(const Unitig &a, const Unitig &b){
		return a._index < b._index;
	}

	static bool UnitigComparator_ByLength(const Unitig &a, const Unitig &b){
		return a._length > b._length;
	}

	//static bool UnitigComparator_BySum_debug(const Unitig &a, const Unitig &b){
	//	return a._debug_nodeIndexSum < b._debug_nodeIndexSum;
	//}

	static bool UnitigComparator_ByLength_Reverse(const Unitig &a, const Unitig &b){
		return a._length < b._length;
	}

	static bool UnitigTipComparator_ByLength_Reverse(const UnitigTip &a, const UnitigTip &b){
        if(a._startNodeIndex == b._startNodeIndex){
            return a._length < b._length;
        }
        return a._startNodeIndex < b._startNodeIndex;
        //if(a._length == b._length){
        //    return a._startNodeIndex < b._startNodeIndex;
        //}
		//return a._length < b._length;
	}
    
	static bool UnitigTipComparator_ByLength(const UnitigTip &a, const UnitigTip &b){

        if(a._length == b._length){
            return a._startNodeIndex > b._startNodeIndex;
        }
        return a._length > b._length;
        //if(a._startNodeIndex == b._startNodeIndex){
        //    return a._length < b._length;
        //}
        //return a._startNodeIndex < b._startNodeIndex;
        //if(a._length == b._length){
        //    return a._startNodeIndex < b._startNodeIndex;
        //}
		//return a._length < b._length;
	}


	static bool BubbleSideComparator(const BubbleSide &a, const BubbleSide &b){

        if(a._nbNodes == b._nbNodes){
            return a._nodeIndexSum > b._nodeIndexSum;
        }
        return a._nbNodes > b._nbNodes;

        //if(a._quality == b._quality){
        //    return a._nodeIndexSum > b._nodeIndexSum;
        //}
        //return a._quality > b._quality;

        //if(a._startNodeIndex == b._startNodeIndex){
        //    return a._length < b._length;
        //}
        //return a._startNodeIndex < b._startNodeIndex;
        //if(a._length == b._length){
        //    return a._startNodeIndex < b._startNodeIndex;
        //}
		//return a._length < b._length;
	}

	static bool UnitigComparator_ByAbundance(const UnitigLength &a, const UnitigLength &b){
		return a._abundance > b._abundance;
	}

	static bool UnitigComparator_ByAbundance2(const Unitig &a, const Unitig &b){
		return a._abundance > b._abundance;
	}

    vector<SaveState2> _cachedGraphStates;
    //unordered_map<float, SaveState> _cachedGraphStates;
    //SaveState _saveState;

    //SaveState saveState(){
    //    return {_isNodeValid2, _isBubble,_isEdgeRemoved, _bubbles, _nodeToUnitig, _unitigs};
    //}

    /*
    void loadState(const SaveState& saveState){
        _isBoundUnitig.clear();
        _removedReadIndex.clear();
        _currentUnitigNodes.clear();
        _isNodeValid2.clear();
        _isNodeInvalid_tmp.clear();
        _isNodeValid2 = saveState._isNodeValid2;
        _isBubble = saveState._isBubble;
        _isEdgeRemoved = saveState._isEdgeRemoved;
        //_bubbles = saveState._bubbles;
        _nodeToUnitig = saveState._nodeToUnitig;
        _unitigs = saveState._unitigs;
        //_isLongUnitig = saveState._isLongUnitig;
        //_longUnitigNodeAbundance = saveState._longUnitigNodeAbundance;
        //_isLongUnitig.clear();
        _longUnitigNodeAbundance.clear();

    }
    */

    void clear(float abundanceCutoff_min){

        _graphSuccessors->clearEdgeRemoved();
        _nextUnitigIndex = 0;
        _unitigs.clear();
        _nodeToUnitig = vector<u_int32_t>(_graphSuccessors->_nbNodes, -1);

        _cleanedLongTips.clear();
        //_isNodenameRoundabout.clear();
        _repeatedNodenames.clear();
        _rareReads.clear();
        _isBoundUnitig.clear();
        _removedReadIndex.clear();
        _currentUnitigNodes.clear();
        //_isLongUnitig.clear();
        _longUnitigNodeAbundance.clear();
        _isNodeInvalid_tmp.clear();
        //_isNodeValid2.clear();
        _isUnitigInvalid_tmp.clear();

        //if(_isNodeValid2_cache.size() == 0){
            //for(size_t nodeIndex=0; nodeIndex<_graphSuccessors->_nbNodes; nodeIndex++){

                //if(_debug_groundTruthNodeNames.size() != 0 && _debug_groundTruthNodeNames.find(BiGraph::nodeIndex_to_nodeName(nodeIndex)) == _debug_groundTruthNodeNames.end()) continue;
                //if(_nodeAbundances[BiGraph::nodeIndex_to_nodeName(nodeIndex)] <= abundanceCutoff_min/3) continue;
                //_isNodeValid2.insert(nodeIndex);
            //}
          //  _isNodeValid2_cache = _isNodeValid2;
        //}
        //else{
            //_isNodeValid2.insert(_isNodeValid2_cache.begin(),_isNodeValid2_cache.end());
            //_isNodeValid2 = _isNodeValid2_cache;
        //}
        /*
        //_isNodeRemoved.resize(_graphSuccessors->_nbNodes, false);
        _isNodeValid = vector<bool>(_graphSuccessors->_nbNodes, true);
        for(size_t nodeIndex=0; nodeIndex<_graphSuccessors->_nbNodes; nodeIndex++){
            if(_nodeAbundances[BiGraph::nodeIndex_to_nodeName(nodeIndex)] <= abundanceCutoff_min/3){
                _isNodeValid[nodeIndex] = false;
            }
        }*/
        
        //_isBubble = vector<bool>(_graphSuccessors->_nbNodes, false);
        _isNodeRemoved = vector<bool>(_graphSuccessors->_nbNodes, false);
        //_isEdgeRemoved.clear();
        _bubbles.clear();
        //_complexAreas_source.clear();
        //_complexAreas_sink.clear();
        
        //_isEdgeRemoved = _isEdgeUnsupported;
        //_logFile << _isEdgeRemoved.size() << endl;
        //_isNodeValid2.erase(BiGraph::nodeName_to_nodeIndex(1009227, false));
        //_isNodeValid2.erase(BiGraph::nodeName_to_nodeIndex(1009227, true));
    }

    /*
    GraphSimplify* clone(){
        GraphSimplify* clone = new GraphSimplify(_inputGfaFilename, _outputDir);

        clone->_graphSuccessors = _graphSuccessors;
        clone->_graphPredecessors = _graphPredecessors;
        clone->_nodeAbundances = _nodeAbundances;
        clone->_nodeLengths = _nodeLengths;
        clone->_isNodeRemoved = _isNodeRemoved;
        clone->_isEdgeRemoved = _isEdgeRemoved;

        return clone;
    }
    */

    /*
    void execute(float abundanceCutoff_min, u_int64_t k){

        clear(0);
        compact(false);

        bool dummy;
        unordered_set<u_int32_t> removedNodes;

        //_logFile << "to remove" << endl;
        //abundanceCutoff_min = 12;
        
        while(true){

            u_int64_t nbTipsRemoved = 0;
            u_int64_t nbErrorRemoved = 0;
            u_int64_t nbBubblesRemoved = 0;
            u_int64_t nbSuperbubblesRemoved = 0;
            u_int64_t nbSmallLoopRemoved = 0;

            bool isModification = false;

            _logFile << "1" << endl;
            while(true){
                _logFile << "lala" << endl;
                compact(true);
                _logFile << "loulou" << endl;
                nbErrorRemoved = removeLowAbundantNodes(abundanceCutoff_min);
                _logFile << "lili" << endl;
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb error removed: " << nbErrorRemoved << endl;
                #endif
                if(nbErrorRemoved == 0) break;
                isModification = true;
            }

            
            
            while(true){
                _logFile << "lala" << endl;
                compact(true);
                _logFile << "loulou" << endl;
                unordered_set<u_int32_t> isTips;
                nbTipsRemoved = tip(4*k, true, isTips);
                nbTipsRemoved = tip(4*k, false, isTips);
                _logFile << "lili" << endl;
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb tip removed: " << nbTipsRemoved << endl;
                #endif
                if(nbTipsRemoved == 0) break;
                isModification = true;
            }
            _logFile << "3" << endl;

            
            while(true){

                u_int64_t nbErrorRemoved = 0;

                compact();

                for(Unitig& unitig : _unitigs){
                    
                    _logFile << unitig._index << " " << unitig._nbNodes << endl;
                    if(unitig._nbNodes >= 2*k) continue;

                    vector<u_int32_t> successors;
                    getSuccessors_unitig(unitig._index, successors);

                    vector<u_int32_t> predecessors;
                    getPredecessors_unitig(unitig._index, predecessors);


                    float neighborAbundanceMin = std::numeric_limits<float>::max();
                    
                    for(u_int32_t unitigIndex : successors){
                        if(_unitigs[unitigIndex]._abundance < neighborAbundanceMin){
                            neighborAbundanceMin = _unitigs[unitigIndex]._abundance;
                        }
                    }
                    for(u_int32_t unitigIndex : predecessors){
                        if(_unitigs[unitigIndex]._abundance < neighborAbundanceMin){
                            neighborAbundanceMin = _unitigs[unitigIndex]._abundance;
                        }
                    }

                    
                    if(unitig._abundance < neighborAbundanceMin*0.5){
                        vector<u_int32_t> unitigNodes;
                        _logFile << "lala" << endl;
                        getUnitigNodes(unitig, unitigNodes);
                        for(u_int32_t node : unitigNodes){
                            _isNodeValid2.erase(node);
                        }
                        _logFile << "loulou" << endl;
                        isModification = true;
                        nbErrorRemoved += 1;
                    }

                }

                if(nbErrorRemoved == 0) break;
            }


            
            while(true){
                compact();
                nbSuperbubblesRemoved = superbubble(50000);
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb superbubble removed: " << nbSuperbubblesRemoved << endl;
                #endif
                if(nbSuperbubblesRemoved == 0) break;
                isModification = true;
            }
            
            
            while(true){
                compact();
                nbBubblesRemoved = bubble(50000);
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb bubble removed: " << nbBubblesRemoved << endl;
                #endif
                if(nbBubblesRemoved == 0) break;
                isModification = true;
            }
            



            _logFile << "4" << endl;
            
            while(true){
                compact(true);
                nbSmallLoopRemoved = removeSmallLoop(10000);
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb small loop removed: " << nbSmallLoopRemoved << endl;
                #endif
                if(nbSmallLoopRemoved == 0) break;
                isModification = true;
            }
            
            
            

            if(!isModification) break;
        }


        

        //exit(1);

        
        for(size_t nodeIndex=0; nodeIndex<_graphSuccessors->_nbNodes; nodeIndex++){
            if(_isNodeValid2.find(nodeIndex) != _isNodeValid2.end()) continue;
            //if(_isNodeValid[nodeIndex]) continue;
            u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex, dummy);
            removedNodes.insert(nodeName);
        }

        _logFile << "REWRITE GFA WITHOUT NODe: faire l'inverse rewrite gfa WITH node et utiliser _isNodeValid2 direct" << endl;
        GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, _inputGfaFilename + "_tmp.gfa", removedNodes, _isEdgeRemoved, _graphSuccessors);
        

    }
    */

   /*
    u_int64_t removeLowAbundantNodes(float abundanceCutoff_min){

        unordered_set<u_int32_t> removedUnitigs;
        
        u_int64_t nbRemoved = 0;

        if(abundanceCutoff_min == 0) return nbRemoved;
        
        unordered_set<u_int32_t> removedNodes;
        for(u_int32_t nodeIndex : _isNodeValid2){
            if(_unitigs[_nodeToUnitig[nodeIndex]]._abundance < abundanceCutoff_min){
                removedNodes.insert(nodeIndex);
                removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
            }
        }

        removeUnitigs(removedUnitigs);
        for(u_int32_t nodeIndex : removedNodes){
            _isNodeValid2.erase(nodeIndex);
            //_removedFrom[nodeIndex] = 5;
        }

        return nbRemoved;
    }
    */

    //unordered_map<u_int32_t, u_int32_t> _removedFrom;

    /*
    u_int64_t tip(float maxLength, bool indexingTips, unordered_set<u_int32_t>& isTips, SaveState2& saveState, bool removeLongTips){

		unordered_set<u_int32_t> writtenUnitigs;


        vector<UnitigTip> unitigTips;

        if(_unitigIndexToClean.size() == 0){
            for(Unitig& u : _unitigs){
                if(u._startNode == -1) continue;
                //if(u._startNode % 2 != 0) continue;

                if(u._length > maxLength) continue;
                //if(removeLongTips){
                //    if(u._length > maxLength) continue;
                //}
                //else{
                //    if(u._nbNodes >= _kminmerSize*2) continue;
                //}


                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._startNode)) != writtenUnitigs.end()) continue;
                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._endNode)) != writtenUnitigs.end()) continue;

                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._startNode));
                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._endNode));
                

                unitigTips.push_back({u._index, u._startNode, u._length});
            }

        }
        else{
            for(u_int32_t unitigIndex : _unitigIndexToClean){
                const Unitig& u = _unitigs[unitigIndex];
                if(u._startNode == -1) continue;
                if(u._length > maxLength) continue;
                unitigTips.push_back({u._index, u._startNode, u._length});
            }
        }


        std::sort(unitigTips.begin(), unitigTips.end(), UnitigTipComparator_ByLength_Reverse);
        

        unordered_set<u_int32_t> removedNodes;

        u_int64_t nbRemoved = 0;

        vector<u_int32_t> neighbors;

        //vector<bool> isVisited(_graphSuccessors->_nbNodes, false);
        bool dummy = false;

        for(const UnitigTip& unitigTip : unitigTips){

            //_logFile << unitigTip._length << endl;

            const Unitig& unitig = _unitigs[unitigTip._unitigIndex];

            //_logFile << unitig._startNode << endl;
        

            //if(removeLongTips){
            //    if(unitig._length > maxLength) continue;
            //}
            //else{
            //    if(unitig._nbNodes >= _kminmerSize*2) continue;
            //}
            //if(unitig._nbNodes >= _kminmerSize*2) continue;
            //if(_isNodeValid2.find(unitig._startNode) == _isNodeValid2.end()) continue; //already removed


            getSuccessors(unitig._endNode, 0, neighbors);
            if(neighbors.size() > 0) continue;

            //if(isVisited[unitig._startNode]) continue;
            //if(isVisited[unitig._endNode]) continue;

            getPredecessors(unitig._startNode, 0, neighbors);
            //if(unitig._index == 116){
            //    _logFile << neighbors.size() << endl;
            //}

            //if(neighbors.size() == 0) continue;



            if(indexingTips){
                if(neighbors.size() == 0) continue;
            }
            else{

                u_int64_t nbPredecessors = 0;
                for(u_int32_t nodeIndex : neighbors){
                    u_int32_t unitigIndex = nodeIndex_to_unitigIndex(nodeIndex);
                    if(isTips.find(unitigIndex) == isTips.end()) nbPredecessors += 1;
                }
                
                //if(nbPredecessors != 1) continue;
                if(nbPredecessors == 0) continue;
                
                //if(nbPredecessors != 0){
                        //if(unitig._length > maxLength) continue;
                    //if(removeLongTips){
                    //    if(unitig._length > maxLength) continue;
                    //}
                    //else{
                    //    if(unitig._nbNodes >= _kminmerSize*2) continue;
                    //}

                    //if(unitig._nbNodes >= _kminmerSize*2){
                    //    continue;
                    //}
                //}
            }




            //if(unitig._index == 116){
            //    _logFile << neighbors.size() << endl;
            //}

            //isVisited[unitig._startNode] = true;
            //isVisited[unitig._endNode] = true;



            //if(removeLongTips && unitig._nbNodes >= _kminmerSize*2){
            //    _cleanedLongTips.push_back(unitig);
            //}

            removedNodes.insert(unitig._startNode);

          

            //if(unitig._index == 116){
            //    _logFile << "omg" << endl;
            //}
            //_logFile << "blabla2" << endl;

            //removedUnitigs.insert(unitig._index);
            //removedUnitigs.insert(unitigIndex_toReverseDirection(unitig._index));

            

            #ifdef PRINT_DEBUG_SIMPLIFICATION
                _logFile << "\tTip: " << _graphSuccessors->nodeIndex_to_nodeName(unitig._endNode, dummy) << " " << unitig._length << endl;
            #endif 

            nbRemoved += 1;
        }

        if(indexingTips) return 0;


        unordered_set<u_int32_t> removedUnitigs;
        for(u_int32_t nodeIndex : removedNodes){
            //if(_nodeToUnitig.find(nodeIndex) == _nodeToUnitig.end()){
            //    _logFile << "KOUERK" << endl;
            //}
            removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
            removedUnitigs.insert(nodeIndex_to_unitigIndex(GraphSimplify::nodeIndex_toReverseDirection(nodeIndex)));
        }
        removeUnitigs(removedUnitigs);
        for(u_int32_t nodeIndexTo : removedNodes){

            vector<u_int32_t> predecessors;
            getPredecessors(nodeIndexTo, 0, predecessors);

            for(u_int32_t nodeIndexFrom : predecessors){
                //_logFile << nodeIndexFrom << " -> " << nodeIndexTo << endl;
			    _graphSuccessors->removeEdge(nodeIndexFrom, nodeIndexTo);
			    _graphSuccessors->removeEdge(GraphSimplify::nodeIndex_toReverseDirection(nodeIndexTo), GraphSimplify::nodeIndex_toReverseDirection(nodeIndexFrom));
            }
            
        }


        return nbRemoved;
    }
    */

    u_int64_t tip(float maxLength, bool indexingTips, SaveState2& saveState, bool lengthIsKminmerSize, bool collectPossibleTips, bool removeTip){

		//unordered_set<u_int32_t> writtenUnitigs;


        vector<UnitigTip> unitigTips;

        if(collectPossibleTips){
            for(const Unitig& u: _unitigs){
                if(u._startNode == -1) continue;

                //if(u._startNode % 2 != 0) continue;

                if(u._length > maxLength) continue;


                unitigTips.push_back({u._index, u._startNode, u._length});
            }
        }
        else{
            
            vector<u_int32_t> removedTips;

            if(_unitigIndexToClean.size() == 0){
                //for(const auto& it: _unitigs){
                //    const Unitig& u = it.second;
                //    if(u._startNode == -1) continue;
                for(u_int32_t unitigIndex : _possibleTips){
                    //if(u._startNode % 2 != 0) continue;

                    
                    const Unitig& u = _unitigs[unitigIndex];
                    if(u._startNode == -1){
                        removedTips.push_back(unitigIndex);
                        continue;
                    }
                    if(u._length > 50000) continue;

                    if(lengthIsKminmerSize){
                        if(u._nbNodes > maxLength) continue;
                    }
                    else{
                        if(u._length > maxLength) continue;
                    }


                    //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._startNode)) != writtenUnitigs.end()) continue;
                    //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._endNode)) != writtenUnitigs.end()) continue;

                    //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._startNode));
                    //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._endNode));
                    

                    unitigTips.push_back({u._index, u._startNode, u._length});
                }

            }
            else{
                for(u_int32_t unitigIndex : _unitigIndexToClean){
                    const Unitig& u = _unitigs[unitigIndex];

                    if(u._startNode == -1){
                        removedTips.push_back(unitigIndex);
                        continue;
                    }

                    //if(u._startNode == -1) continue;
                    if(u._length > 50000) continue;

                    if(lengthIsKminmerSize){
                        if(u._nbNodes > maxLength) continue;
                    }
                    else{
                        if(u._length > maxLength) continue;
                    }
                    unitigTips.push_back({u._index, u._startNode, u._length});
                }
            }

            for(u_int32_t unitigIndex : removedTips){
                _possibleTips.erase(unitigIndex);
            }
        }



        //std::sort(unitigTips.begin(), unitigTips.end(), UnitigTipComparator_ByLength_Reverse);
        

        unordered_set<u_int32_t> removedNodes;

        u_int64_t nbRemoved = 0;

        //vector<u_int32_t> neighbors;

        //vector<bool> isVisited(_graphSuccessors->_nbNodes, false);
        //bool dummy = false;


        /*
        for(const UnitigTip& unitigTip : unitigTips){

            //_logFile << unitigTip._length << endl;

            const Unitig& unitig = _unitigs[unitigTip._unitigIndex];

            //_logFile << unitig._startNode << endl;
        

            //if(removeLongTips){
            //    if(unitig._length > maxLength) continue;
            //}
            //else{
            //    if(unitig._nbNodes >= _kminmerSize*2) continue;
            //}
            //if(unitig._nbNodes >= _kminmerSize*2) continue;
            //if(_isNodeValid2.find(unitig._startNode) == _isNodeValid2.end()) continue; //already removed


            getSuccessors(unitig._endNode, 0, neighbors);
            if(neighbors.size() > 0) continue;

            //if(isVisited[unitig._startNode]) continue;
            //if(isVisited[unitig._endNode]) continue;

            getPredecessors(unitig._startNode, 0, neighbors);
            //if(unitig._index == 116){
            //    _logFile << neighbors.size() << endl;
            //}

            //if(neighbors.size() == 0) continue;



            if(indexingTips){
                if(neighbors.size() == 0) continue;
            }
            else{

                u_int64_t nbPredecessors = 0;
                for(u_int32_t nodeIndex : neighbors){
                    u_int32_t unitigIndex = nodeIndex_to_unitigIndex(nodeIndex);
                    if(isTips.find(unitigIndex) == isTips.end()) nbPredecessors += 1;
                }
                
                //if(nbPredecessors != 1) continue;
                if(nbPredecessors == 0) continue;
                
                //if(nbPredecessors != 0){
                        //if(unitig._length > maxLength) continue;
                    //if(removeLongTips){
                    //    if(unitig._length > maxLength) continue;
                    //}
                    //else{
                    //    if(unitig._nbNodes >= _kminmerSize*2) continue;
                    //}

                    //if(unitig._nbNodes >= _kminmerSize*2){
                    //    continue;
                    //}
                //}
            }




            //if(unitig._index == 116){
            //    _logFile << neighbors.size() << endl;
            //}

            //isVisited[unitig._startNode] = true;
            //isVisited[unitig._endNode] = true;
            */
            /*
            getUnitigNodes(unitig, unitigNodes);
            for(u_int32_t node : unitigNodes){
                _isNodeValid2.erase(node);
                _isNodeValid2.erase(nodeIndex_toReverseDirection(node));

                //_isNodeValid[node] = false;
            }
            */

            /*
            if(indexingTips){
                isTips.insert(unitig._index);
                isTips.insert(unitigIndex_toReverseDirection(unitig._index));
                continue;
            }
            */

            //if(removeLongTips && unitig._nbNodes >= _kminmerSize*2){
            //    _cleanedLongTips.push_back(unitig);
            //}

            //removedNodes.insert(unitig._startNode);

            /*
            vector<u_int32_t> unitigNodes; 
            getUnitigNodes(unitig, unitigNodes);
            for(u_int32_t node : unitigNodes){
                //if(_isNodeValid2.find(node) == _isNodeValid2.end()){
                //    _logFile << "omg1" << endl;
                //    getchar();
                //}
                //if(_isNodeValid2.find(nodeIndex_toReverseDirection(node)) == _isNodeValid2.end()){
                //    _logFile << (_removedFrom.find(nodeIndex_toReverseDirection(node)) != _removedFrom.end()) << endl;
                //    _logFile << _removedFrom[nodeIndex_toReverseDirection(node)] << endl;
                //    _logFile << "omg2" << endl;
                //    getchar();
                //}
                removedNodes.insert(node);
                removedNodes.insert(nodeIndex_toReverseDirection(node));
                saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(node));
            }
            */

            //if(unitig._index == 116){
            //    _logFile << "omg" << endl;
            //}
            //_logFile << "blabla2" << endl;

            //removedUnitigs.insert(unitig._index);
            //removedUnitigs.insert(unitigIndex_toReverseDirection(unitig._index));

            /*
            _availableUnitigIndexes.insert(nodeIndex_to_unitigIndex(node));
            _availableUnitigIndexes.insert(nodeIndex_to_unitigIndex(nodeIndex_toReverseDirection(node)));

            getSuccessors_unitig(unitig._index, neighbors);
            for(u_int32_t unitigIndex : neighbors){
                _rebuildInvalidUnitigs.insert(unitigIndex);
                _rebuildInvalidUnitigs.insert(unitigIndex_toReverseDirection(unitigIndex));
            }
            getPredecessors_unitig(unitig._index, neighbors);
            for(u_int32_t unitigIndex : neighbors){
                _rebuildInvalidUnitigs.insert(unitigIndex);
                _rebuildInvalidUnitigs.insert(unitigIndex_toReverseDirection(unitigIndex));
            }
            */
            

            //#ifdef PRINT_DEBUG_SIMPLIFICATION
            //    _logFile << "\tTip: " << _graphSuccessors->nodeIndex_to_nodeName(unitig._endNode, dummy) << " " << unitig._length << endl;
            //#endif 

            //nbRemoved += 1;
        //}

        //if(indexingTips) return 0;



        TipFunctor functor(this, removedNodes, collectPossibleTips, removeTip);
        size_t i = 0;

        #pragma omp parallel num_threads(_nbCores)
        {

            TipFunctor functorSub(functor);

            while(true){

                u_int32_t unitigIndex = -1;
                bool isEof = false;

                #pragma omp critical
                {
                    
                    if(i >= unitigTips.size()){
                        isEof = true;
                    }
                    else{
                        unitigIndex = unitigTips[i]._unitigIndex;
                        //_logFile << nodeIndex << endl;
                    }

                    i += 1;
                }

                if(isEof) break;
                functorSub(unitigIndex);

            }
            

        }

        //for(const UnitigTip& unitigTip : unitigTips){

        //const Unitig& unitig = _unitigs[unitigTip._unitigIndex];
        //}


        unordered_set<u_int32_t> removedUnitigs;
        for(u_int32_t nodeIndex : removedNodes){
            //if(_nodeToUnitig.find(nodeIndex) == _nodeToUnitig.end()){
            //    _logFile << "KOUERK" << endl;
            //}
            removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
            removedUnitigs.insert(nodeIndex_to_unitigIndex(GraphSimplify::nodeIndex_toReverseDirection(nodeIndex)));
        }
        removeUnitigs(removedUnitigs);

        if(!removeTip){
            for(u_int32_t nodeIndexTo : removedNodes){

                //_logFile << "Removed: " << BiGraph::nodeIndex_to_nodeName(nodeIndexTo) << endl;

                vector<u_int32_t> predecessors;
                getPredecessors(nodeIndexTo, 0, predecessors);

                for(u_int32_t nodeIndexFrom : predecessors){

                    
                    //DbgEdge edge = {nodeIndexFrom, nodeIndexTo};
                    //edge = edge.normalize();
                    //_isEdgeRemoved.insert(edge);
                    saveState._isEdgeRemoved.push_back({nodeIndexFrom, nodeIndexTo});

                    //DbgEdge edgeRC = {GraphSimplify::nodeIndex_toReverseDirection(nodeIndexTo), GraphSimplify::nodeIndex_toReverseDirection(nodeIndexFrom)};
                    //edgeRC = edgeRC.normalize();
                    //_isEdgeRemoved.insert(edgeRC);
                    //saveState._isEdgeRemoved.push_back(edgeRC);

                    _tip_checksum += BiGraph::nodeIndex_to_nodeName(nodeIndexFrom) + BiGraph::nodeIndex_to_nodeName(nodeIndexTo);
                    

                    //_logFile << nodeIndexFrom << " -> " << nodeIndexTo << endl;
                    _graphSuccessors->setEdgeRemoved(nodeIndexFrom, nodeIndexTo, true);
                    _graphSuccessors->setEdgeRemoved(GraphSimplify::nodeIndex_toReverseDirection(nodeIndexTo), GraphSimplify::nodeIndex_toReverseDirection(nodeIndexFrom), true);
                    //if(!found || !found2){
                    //    _logFile << "HAAAAA" << endl;
                    //    getchar();
                    //}
                }

                //_possibleTips.erase(nodeIndex_to_unitigIndex(nodeIndexTo));


                
            }
        }
        else{
            for(u_int32_t nodeIndex : removedNodes){

                _isNodeRemoved[nodeIndex] = true;
                _isNodeRemoved[nodeIndex_toReverseDirection(nodeIndex)] = true;
                //_logFile << (_isNodeValid2.find(nodeIndex) != _isNodeValid2.end()) << " " << (_isNodeValid2.find(nodeIndex_toReverseDirection(nodeIndex)) != _isNodeValid2.end()) << endl;
                //_isNodeValid2.erase(nodeIndex);
                //_isNodeValid2.erase(nodeIndex_toReverseDirection(nodeIndex));
                saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex));


            }
        }
        /*
        for(u_int32_t nodeIndex : removedNodes){
            //_removedFrom[nodeIndex] = 1;
            _isNodeValid2.erase(nodeIndex);
            //file_debug << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "green" << endl;
        }
        */
        nbRemoved = removedUnitigs.size();
        return nbRemoved;
    }

    bool isTip(u_int32_t unitigIndex){
        return isTip(_unitigs[unitigIndex]) || isTip(_unitigs[unitigIndex_toReverseDirection(unitigIndex)]);
    }

    bool isTip(const Unitig& u){

        //const Unitig& u = _unitigs[unitigIndex];

        vector<u_int32_t> neighbors;
        getSuccessors(u._endNode, 0, neighbors);
        if(neighbors.size() > 0) return false;


        getPredecessors(u._startNode, 0, neighbors);

        if(neighbors.size() == 0) return false;

        return true;
    }

	class TipFunctor {

		public:

		GraphSimplify* _graph;
        unordered_set<u_int32_t>& _removedNodes;
        bool _collectPossibleTips;
        bool _removeTip;

		TipFunctor(GraphSimplify* graph, unordered_set<u_int32_t>& removedNodes, bool collectPossibleTips, bool removeTip) : _graph(graph), _removedNodes(removedNodes){
            _collectPossibleTips = collectPossibleTips;
            _removeTip = removeTip;
        }

		TipFunctor(const TipFunctor& copy) : _graph(copy._graph), _removedNodes(copy._removedNodes){
            _collectPossibleTips = copy._collectPossibleTips;
            _removeTip = copy._removeTip;
		}

		~TipFunctor(){
		}

		void operator () (u_int32_t unitigIndex) {

            const Unitig& unitig = _graph->_unitigs[unitigIndex];
            if(!_graph->isTip(unitig._index)) return;

            /*
            vector<u_int32_t> neighbors;

            _graph->getSuccessors(unitig._endNode, 0, neighbors);
            if(neighbors.size() > 0) return;

            //if(isVisited[unitig._startNode]) continue;
            //if(isVisited[unitig._endNode]) continue;

            _graph->getPredecessors(unitig._startNode, 0, neighbors);
            //if(unitig._index == 116){
            //    _logFile << neighbors.size() << endl;
            //}

            //if(neighbors.size() == 0) continue;



            //if(indexingTips){
            //    if(neighbors.size() == 0) continue;
            //}
            //else{

                //u_int64_t nbPredecessors = 0;
                //for(u_int32_t nodeIndex : neighbors){
                //    u_int32_t unitigIndex = _graph->nodeIndex_to_unitigIndex(nodeIndex);
                    //if(isTips.find(unitigIndex) == isTips.end()) nbPredecessors += 1;
                //}
                
                //if(nbPredecessors != 1) continue;
                if(neighbors.size() == 0) return;
                
                //if(nbPredecessors != 0){
                        //if(unitig._length > maxLength) continue;
                    //if(removeLongTips){
                    //    if(unitig._length > maxLength) continue;
                    //}
                    //else{
                    //    if(unitig._nbNodes >= _kminmerSize*2) continue;
                    //}

                    //if(unitig._nbNodes >= _kminmerSize*2){
                    //    continue;
                    //}
                //}
            //}
            */



            //if(unitig._index == 116){
            //    _logFile << neighbors.size() << endl;
            //}

            //isVisited[unitig._startNode] = true;
            //isVisited[unitig._endNode] = true;

            /*
            getUnitigNodes(unitig, unitigNodes);
            for(u_int32_t node : unitigNodes){
                _isNodeValid2.erase(node);
                _isNodeValid2.erase(nodeIndex_toReverseDirection(node));

                //_isNodeValid[node] = false;
            }
            */

            /*
            if(indexingTips){
                isTips.insert(unitig._index);
                isTips.insert(unitigIndex_toReverseDirection(unitig._index));
                continue;
            }
            */

            //if(removeLongTips && unitig._nbNodes >= _kminmerSize*2){
            //    _cleanedLongTips.push_back(unitig);
            //}

            #pragma omp critical(tip)
            {
                if(_collectPossibleTips){
                    _graph->_possibleTips.insert(unitig._index);
                }
                else{
                    if(_removeTip){
                        for(u_int32_t nodeIndex : _graph->_unitigs[unitig._index]._nodes){
                            _removedNodes.insert(nodeIndex);
                        }
                    }
                    else{
                        _removedNodes.insert(unitig._startNode);
                    }
                }
            }

            /*
            vector<u_int32_t> unitigNodes; 
            getUnitigNodes(unitig, unitigNodes);
            for(u_int32_t node : unitigNodes){
                //if(_isNodeValid2.find(node) == _isNodeValid2.end()){
                //    _logFile << "omg1" << endl;
                //    getchar();
                //}
                //if(_isNodeValid2.find(nodeIndex_toReverseDirection(node)) == _isNodeValid2.end()){
                //    _logFile << (_removedFrom.find(nodeIndex_toReverseDirection(node)) != _removedFrom.end()) << endl;
                //    _logFile << _removedFrom[nodeIndex_toReverseDirection(node)] << endl;
                //    _logFile << "omg2" << endl;
                //    getchar();
                //}
                removedNodes.insert(node);
                removedNodes.insert(nodeIndex_toReverseDirection(node));
                saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(node));
            }
            */

            //if(unitig._index == 116){
            //    _logFile << "omg" << endl;
            //}
            //_logFile << "blabla2" << endl;

            //removedUnitigs.insert(unitig._index);
            //removedUnitigs.insert(unitigIndex_toReverseDirection(unitig._index));

            /*
            _availableUnitigIndexes.insert(nodeIndex_to_unitigIndex(node));
            _availableUnitigIndexes.insert(nodeIndex_to_unitigIndex(nodeIndex_toReverseDirection(node)));

            getSuccessors_unitig(unitig._index, neighbors);
            for(u_int32_t unitigIndex : neighbors){
                _rebuildInvalidUnitigs.insert(unitigIndex);
                _rebuildInvalidUnitigs.insert(unitigIndex_toReverseDirection(unitigIndex));
            }
            getPredecessors_unitig(unitig._index, neighbors);
            for(u_int32_t unitigIndex : neighbors){
                _rebuildInvalidUnitigs.insert(unitigIndex);
                _rebuildInvalidUnitigs.insert(unitigIndex_toReverseDirection(unitigIndex));
            }
            */
            


        }
    };
    u_int64_t removeSelfLoops(SaveState2& saveState){

        unordered_set<u_int32_t> removedNodes;

        u_int64_t nbRemoved = 0;

        vector<u_int32_t> neighbors;

        //vector<bool> isVisited(_graphSuccessors->_nbNodes, false);
        bool dummy = false;

		unordered_set<u_int32_t> writtenUnitigs;

        vector<UnitigTip> unitigTips;

        if(_unitigIndexToClean.size() == 0){
            for(const Unitig& u: _unitigs){
                if(u._startNode == -1) continue;
                if(u._nbNodes != 1) continue;
                //if(u._startNode % 2 != 0) continue;

                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._startNode)) != writtenUnitigs.end()) continue;
                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._endNode)) != writtenUnitigs.end()) continue;

                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._startNode));
                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._endNode));
                

                unitigTips.push_back({u._index, u._startNode, u._length});
            }

        }
        else{

            for(u_int32_t unitigIndex : _unitigIndexToClean){
                const Unitig& u = _unitigs[unitigIndex];
                if(u._startNode == -1) continue;
                if(u._nbNodes != 1) continue;
                //if(u._length > maxLength) continue;
                unitigTips.push_back({u._index, u._startNode, u._length});
            }
        }

        //std::sort(unitigTips.begin(), unitigTips.end(), UnitigTipComparator_ByLength_Reverse);
        

        for(const UnitigTip& unitigTip : unitigTips){


            const Unitig& unitig = _unitigs[unitigTip._unitigIndex];

        //for(Unitig& unitig : _unitigs){
        //    if(unitig._startNode == -1) continue;


            //if(unitig._length > maxLength) continue;
            //if(_isNodeValid2.find(unitig._startNode) == _isNodeValid2.end()) continue; //already removed


            //if(isVisited[unitig._startNode]) continue;
            //if(isVisited[unitig._endNode]) continue;

            getPredecessors(unitig._startNode, 0, neighbors);
            if(neighbors.size() != 1) continue;
            u_int32_t nodeName_pred = BiGraph::nodeIndex_to_nodeName(neighbors[0]);

            getSuccessors(unitig._startNode, 0, neighbors);
            if(neighbors.size() != 1) continue;
            u_int32_t nodeName_succ = BiGraph::nodeIndex_to_nodeName(neighbors[0]);

            if(nodeName_pred != nodeName_succ) continue;


            u_int32_t nodeIndex_pred = neighbors[0];

            vector<u_int32_t> unitigNodes; 
            getUnitigNodes(unitig, unitigNodes);
            for(u_int32_t node : unitigNodes){
                removedNodes.insert(node);
                removedNodes.insert(nodeIndex_toReverseDirection(node));
                //saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(node));
            }

            

            #ifdef PRINT_DEBUG_SIMPLIFICATION
                _logFile << "\tSelf loop: " << BiGraph::nodeIndex_to_nodeName(unitig._endNode, dummy) << endl;
            #endif 

            nbRemoved += 1;
        }

        for(u_int32_t nodeIndexTo : removedNodes){

            
            //DbgEdge edge = {nodeIndexTo, nodeIndexTo};
            //edge = edge.normalize();
            //_isEdgeRemoved.insert(edge);
            saveState._isEdgeRemoved.push_back({nodeIndexTo, nodeIndexTo});
            
            _graphSuccessors->setEdgeRemoved(nodeIndexTo, nodeIndexTo, true);
            _graphSuccessors->setEdgeRemoved(GraphSimplify::nodeIndex_toReverseDirection(nodeIndexTo), GraphSimplify::nodeIndex_toReverseDirection(nodeIndexTo), true);
            /*
            vector<u_int32_t> predecessors;
            getPredecessors(nodeIndexTo, 0, predecessors);

            for(u_int32_t nodeIndexFrom : predecessors){

                
                DbgEdge edge = {nodeIndexFrom, nodeIndexTo};
                edge = edge.normalize();
                _isEdgeRemoved.insert(edge);
                saveState._isEdgeRemoved.push_back(edge);

                DbgEdge edgeRC = {GraphSimplify::nodeIndex_toReverseDirection(nodeIndexTo), GraphSimplify::nodeIndex_toReverseDirection(nodeIndexFrom)};
                edgeRC = edgeRC.normalize();
                _isEdgeRemoved.insert(edgeRC);
                saveState._isEdgeRemoved.push_back(edgeRC);
                

                //_logFile << nodeIndexFrom << " -> " << nodeIndexTo << endl;
			    //_graphSuccessors->removeEdge(nodeIndexFrom, nodeIndexTo);
			    //_graphSuccessors->removeEdge(GraphSimplify::nodeIndex_toReverseDirection(nodeIndexTo), GraphSimplify::nodeIndex_toReverseDirection(nodeIndexFrom));
            }
            */
            
            
        }


        unordered_set<u_int32_t> removedUnitigs;
        for(u_int32_t nodeIndex : removedNodes){
            removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
        }
        removeUnitigs(removedUnitigs);

        //for(u_int32_t nodeIndex : removedNodes){
        //    _isNodeValid2.erase(nodeIndex);
        //}

        return nbRemoved;
    }

    /*
    u_int64_t removeLongTips(){

        unordered_set<u_int32_t> validUnitigs;
        u_int64_t nbRemoved = 0;
        unordered_set<u_int32_t> removedUnitigs;
        unordered_set<u_int32_t> removedNodes;

        for(Unitig& unitig : _unitigs){
            
            if(unitig._startNode == -1) continue;

            if(removedUnitigs.find(unitig._index) != removedUnitigs.end()) continue;
            if(validUnitigs.find(unitig._index) != validUnitigs.end()) continue;

            _logFile << unitig._index << endl;

            vector<vector<u_int32_t>> components;
            getStronglyConnectedComponents(unitig._index, true, components);

            for(const vector<u_int32_t>& component : components){
                if(component.size() == 1){
                    Unitig& unitigSingle = _unitigs[component[0]];
                    if(unitigSingle._startNode != unitigSingle._endNode){

                        vector<u_int32_t> unitigNodes; 
                        getUnitigNodes(unitigSingle, unitigNodes);
                        for(u_int32_t node : unitigNodes){
                            removedNodes.insert(node);
                            removedNodes.insert(nodeIndex_toReverseDirection(node));
                            removedUnitigs.insert(nodeIndex_to_unitigIndex(node));
                            removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex_toReverseDirection(node)));
                        }

                        nbRemoved += 1;
                    }
                    else{
                        validUnitigs.insert(component[0]);
                        validUnitigs.insert(unitigIndex_toReverseDirection(component[0]));
                    }
                }
                else{
                    for(u_int32_t unitigIndexCirc : component){
                        validUnitigs.insert(unitigIndexCirc);
                        validUnitigs.insert(unitigIndex_toReverseDirection(unitigIndexCirc));
                    }
                }
            }
        }


        removeUnitigs(removedUnitigs);
        for(u_int32_t nodeIndex : removedNodes){
            //_removedFrom[nodeIndex] = 1;
            _isNodeValid2.erase(nodeIndex);
        }




        return nbRemoved;
    }

    //void isLongTip(u_int32_t unitigIndex){

    //}*/

    void removeUnitigs(unordered_set<u_int32_t>& unitigs){

        _removedUnitigs.clear();
        unordered_set<u_int32_t> allInvalidUnitigs;

        //auto start = high_resolution_clock::now();
        for(u_int32_t unitigIndex : unitigs){
            collectInvalidUnitig2(unitigIndex, allInvalidUnitigs);
        }

        //auto stop = high_resolution_clock::now();
        //_logFile << "Duration: " << duration_cast<seconds>(stop - start).count() << endl;

        for(u_int32_t unitigIndex : allInvalidUnitigs){
            disableUnitig(unitigIndex);
        }


    }

    void collectInvalidUnitig(u_int32_t unitigIndex, unordered_set<u_int32_t>& allInvalidUnitigs){
        //_removedUnitigs.insert(unitigIndex);
        collectInvalidUnitig2(unitigIndex, allInvalidUnitigs);

        vector<u_int32_t> neighbors;

        getSuccessors_unitig(unitigIndex, 0, neighbors);
        
        for(u_int32_t unitigIndex_nn : neighbors){
            collectInvalidUnitig2(unitigIndex_nn, allInvalidUnitigs);
            //_rebuildInvalidUnitigs.insert(unitigIndex_toReverseDirection(unitigIndex));
        }
        getPredecessors_unitig(unitigIndex, 0, neighbors);
        for(u_int32_t unitigIndex_nn : neighbors){
            collectInvalidUnitig2(unitigIndex_nn, allInvalidUnitigs);
            //_rebuildInvalidUnitigs.insert(unitigIndex);
            //_rebuildInvalidUnitigs.insert(unitigIndex_toReverseDirection(unitigIndex));
        }
    }

    void collectInvalidUnitig2(u_int32_t unitigIndex, unordered_set<u_int32_t>& allInvalidUnitigs){
        allInvalidUnitigs.insert(unitigIndex);
        //allInvalidUnitigs.insert(unitigIndex_toReverseDirection(unitigIndex));
        vector<u_int32_t> neighbors;

        getSuccessors_unitig(unitigIndex, 0, neighbors);
        for(u_int32_t unitigIndex_nn : neighbors){
            allInvalidUnitigs.insert(unitigIndex_nn);
            //allInvalidUnitigs.insert(unitigIndex_toReverseDirection(unitigIndex_nn));
        }
        getPredecessors_unitig(unitigIndex, 0, neighbors);
        for(u_int32_t unitigIndex_nn : neighbors){
            allInvalidUnitigs.insert(unitigIndex_nn);
            //allInvalidUnitigs.insert(unitigIndex_toReverseDirection(unitigIndex_nn));
        }

    }

    void disableUnitig(u_int32_t unitigIndex){
        //_logFile << _unitigs[unitigIndex]._startNode << endl;
        //_logFile << _unitigs[unitigIndex]._endNode << endl;
        //if(_unitigs[unitigIndex]._startNode == -1) return;
        //_rebuildInvalidUnitigs.insert(unitigIndex);
        //_availableUnitigIndexes.insert(unitigIndex);


        //vector<u_int32_t> unitigNodes; 
        //getUnitigNodes(_unitigs[unitigIndex], unitigNodes);
        for(u_int32_t nodeIndex : _unitigs[unitigIndex]._nodes){
            _nodeToUnitig[nodeIndex] = -1;
            _nodeToUnitig[nodeIndex_toReverseDirection(nodeIndex)] = -1;

            //if(nodeIndex == 7294223){
                //_logFile << "Disabled: " << unitigIndex << endl;
                //getchar();
            //}

        }


        //_unitigs.erase(unitigIndex);
        u_int32_t unitigIndexRc = unitigIndex_toReverseDirection(unitigIndex);

        _unitigs[unitigIndex]._startNode = -1;
        _unitigs[unitigIndex]._endNode = -1;
        _unitigs[unitigIndexRc]._startNode = -1;
        _unitigs[unitigIndexRc]._endNode = -1;
        _removedUnitigs.insert(unitigIndex);
        _removedUnitigs.insert(unitigIndexRc);
        //_unitigs[unitigIndex]._nodes;

    }

    unordered_set<u_int32_t> _superbubble_invalidUnitigs;

    u_int64_t superbubble(u_int64_t maxLength, vector<bool>& isBubble, SaveState2& saveState, bool useReadpathSubgraph, GraphSimplify* graphMain, bool withCycle){

        _superbubble_invalidUnitigs.clear();

        _nextReadIndexWriter = 0;
	    _readWriterQueue = priority_queue<SuperbubbleWriter, vector<SuperbubbleWriter> , SuperbubbleWriter_Comparator>();

        unordered_set<u_int32_t> removedNodes;
        //unordered_set<u_int32_t> removedUnitigs;

        #ifdef PRINT_DEBUG_SIMPLIFICATION
            _logFile << "\tStart superbubble" << endl;
        #endif 

        u_int64_t nbRemoved = 0;
		//unordered_set<u_int32_t> writtenUnitigs;


        vector<UnitigTip> unitigTips;

        if(_unitigIndexToClean.size() == 0){
            for(const Unitig& u: _unitigs){
                if(u._startNode == -1) continue;
                //if(u._nbNodes <= _kminmerSize-1) continue;
                //if(u._index % 2 == 1) continue;

                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._startNode)) != writtenUnitigs.end()) continue;
                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._endNode)) != writtenUnitigs.end()) continue;

                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._startNode));
                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._endNode));
                

                unitigTips.push_back({u._index, u._startNode, u._length});
            }

        }
        else{

            for(u_int32_t unitigIndex : _unitigIndexToClean){
                const Unitig& u = _unitigs[unitigIndex];
                if(u._startNode == -1) continue;
                //if(u._nbNodes <= _kminmerSize-1) continue;
                //if(u._index % 2 == 1) continue;
                //if(u._length > maxLength) continue;
                unitigTips.push_back({u._index, u._startNode, u._length});
            }
        }


        //srand(time(NULL));
        //std::random_shuffle(unitigTips.begin(), unitigTips.end());
        std::sort(unitigTips.begin(), unitigTips.end(), UnitigTipComparator_ByLength);

        SuperbubbleFunctor functor(this, removedNodes, isBubble, maxLength, saveState);
        u_int64_t i = 0;

        #pragma omp parallel num_threads(_nbCores)
        {

            SuperbubbleFunctor functorSub(functor);
            SuperbubbleWriter sw;

            while(true){

                u_int32_t unitigIndex = -1;
                bool isEof = false;

                #pragma omp critical
                {
                    
                    if(i >= unitigTips.size()){
                        isEof = true;
                    }
                    else{
                        unitigIndex = unitigTips[i]._unitigIndex;
                        //_logFile << _unitigs[unitigIndex]._nbNodes << endl;
                        //_logFile << nodeIndex << endl;
                    }
                    sw = {i, unitigIndex, {}};
                    i += 1;
                    //_logFile << sw._readIndex << endl;
                }

                if(isEof) break;
                functorSub(sw);

            }
            

        }


        //std::sort(unitigTips.begin(), unitigTips.end(), UnitigTipComparator_ByLength_Reverse);
        
        /*
        for(const UnitigTip& unitigTip : unitigTips){


            const Unitig& unitig = _unitigs[unitigTip._unitigIndex];
        //_logFile << "Super bubble!" << endl;
        //for(Unitig& unitig : _unitigs){
            //if(unitig._startNode == -1) continue;
            //if(BiGraph::nodeIndex_to_nodeName(unitig._endNode) != 1307) continue;
            //_logFile << BiGraph::nodeIndex_to_nodeName(unitig._endNode) << endl;
            
            if(isBubble[unitig._endNode]) continue;
            if(isBubble[nodeIndex_toReverseDirection(unitig._endNode)]) continue;
 
    
            vector<u_int32_t> successors;
            getSuccessors_unitig(unitig._index, 0, successors);
            if(successors.size() <= 1) continue;

            u_int32_t unitigIndex_exit = -1;
            if(withCycle){
                unitigIndex_exit = detectSuperbubble_withCycle(unitig._index, maxLength, isBubble);

                continue;
            }
            else{
                unitigIndex_exit = detectSuperbubble(unitig._index, maxLength, isBubble);
            }

            if(unitigIndex_exit == -1) continue;
            if(unitigIndex_exit == unitig._index || unitigIndex_exit == unitigIndex_toReverseDirection(unitig._index)) continue; //loop side of an inverse repeat
            if(isBubble[_unitigs[unitigIndex_exit]._startNode]) continue;
            if(isBubble[nodeIndex_toReverseDirection(_unitigs[unitigIndex_exit]._startNode)]) continue;

            //_logFile << unitig._startNode << endl;

            #ifdef PRINT_DEBUG_SIMPLIFICATION
                _logFile << "\tSuperbubble: " << BiGraph::nodeToString(unitig._endNode) << " " << BiGraph::nodeToString(_unitigs[unitigIndex_exit]._startNode) << endl;
                _logFile << "\tSuperbubble: " << BiGraph::nodeToString(nodeIndex_toReverseDirection(unitig._endNode)) << " " << BiGraph::nodeToString(nodeIndex_toReverseDirection(_unitigs[unitigIndex_exit]._startNode)) << endl;
                //getchar();
            #endif

            //_logFile << "Found superbubble: " << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_exit]._startNode) << endl;
            simplifySuperbubble(unitig._index, unitigIndex_exit, removedNodes, isBubble, saveState);
            

            nbRemoved += 1;
        }
        */

        //_logFile << removedNodes.size() << endl;
        //getchar();

        unordered_set<u_int32_t> removedUnitigs;
        for(u_int32_t nodeIndex : removedNodes){
            removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
        }
        removeUnitigs(removedUnitigs);
        for(u_int32_t nodeIndex : removedNodes){

            _isNodeRemoved[nodeIndex] = true;
            //_removedFrom[nodeIndex] = 2;
            //_isNodeValid2.erase(nodeIndex);
            //file_debug << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "green" << endl;
            //if(395881 == BiGraph::nodeIndex_to_nodeName(nodeIndex)){
              //      _logFile << "lala5" << endl;
               //     getchar();
                //}
        }

        //_logFile << removedNodes.size() << " " << removedUnitigs.size() << endl;
        //getchar();
        //exit(1);
        return removedUnitigs.size();
    }

    struct SuperbubbleWriter{
        u_int64_t _readIndex;
        u_int32_t _unitigIndex_source;
        vector<u_int32_t> _nodeIndexRemoved;
        //u_int32_t _prevNodeIndex;
    };

    struct SuperbubbleWriter_Comparator {
        bool operator()(SuperbubbleWriter const& p1, SuperbubbleWriter const& p2){
            return p1._readIndex > p2._readIndex;
        }
    };


	priority_queue<SuperbubbleWriter, vector<SuperbubbleWriter> , SuperbubbleWriter_Comparator> _readWriterQueue;
	u_int64_t _nextReadIndexWriter;


	class SuperbubbleFunctor {

		public:

		GraphSimplify* _graph;
        unordered_set<u_int32_t>& _removedNodes;
        vector<bool>& _isBubble;
        vector<Unitig>& _unitigs;
        SaveState2& _saveState;
        u_int64_t _maxLength;

		SuperbubbleFunctor(GraphSimplify* graph, unordered_set<u_int32_t>& removedNodes, vector<bool>& isBubble, u_int64_t maxLength, SaveState2& saveState) : _graph(graph), _removedNodes(removedNodes), _isBubble(isBubble), _unitigs(graph->_unitigs), _saveState(saveState){
            _maxLength = maxLength;
        }

		SuperbubbleFunctor(const SuperbubbleFunctor& copy) : _graph(copy._graph), _removedNodes(copy._removedNodes), _isBubble(copy._isBubble), _unitigs(copy._graph->_unitigs), _saveState(copy._saveState){
            _maxLength = copy._maxLength;
		}

		~SuperbubbleFunctor(){
		}

		void operator () (SuperbubbleWriter sw) {
            
            u_int32_t unitigIndex = sw._unitigIndex_source;
            const Unitig& unitig = _graph->_unitigs[unitigIndex];
            
            bool exist = false;
            #pragma omp critical(superbubble)
            {
                //_logFile << "Try: " << unitig._startNode << endl;
                if(_isBubble[unitig._endNode]) exist = true;
                if(_isBubble[nodeIndex_toReverseDirection(unitig._endNode)]) exist = true;
            }
            if(exist){
                _graph->simplifySuperbubble(sw, _removedNodes, _isBubble, _saveState);
                return;
            }
 
    
            vector<u_int32_t> successors;
            _graph->getSuccessors_unitig(unitig._index, 0, successors);
            if(successors.size() <= 1){
                _graph->simplifySuperbubble(sw, _removedNodes, _isBubble, _saveState);
                return;
            }

            u_int32_t unitigIndex_exit = -1;
            //if(withCycle){
            //    unitigIndex_exit = detectSuperbubble_withCycle(unitig._index, maxLength, isBubble);

            //    continue;
            //}
            //else{
            //_logFile << "Try2: " << unitig._startNode << endl;
            unitigIndex_exit = detectSuperbubble(unitig._index, _maxLength, _isBubble);
            //}

            if(unitigIndex_exit == -1){
                _graph->simplifySuperbubble(sw, _removedNodes, _isBubble, _saveState);
                return;
            }
            if(unitigIndex_exit == unitig._index || unitigIndex_exit == _graph->unitigIndex_toReverseDirection(unitig._index)){ //loop side of an inverse repeat
                _graph->simplifySuperbubble(sw, _removedNodes, _isBubble, _saveState);
                return;
            }
            //if(_isBubble[_unitigs[unitigIndex_exit]._startNode]) return;
            //if(_isBubble[nodeIndex_toReverseDirection(_unitigs[unitigIndex_exit]._startNode)]) return;
            exist = false;
            #pragma omp critical(superbubble)
            {
                if(_isBubble[_unitigs[unitigIndex_exit]._startNode]) exist = true;
                if(_isBubble[nodeIndex_toReverseDirection(_unitigs[unitigIndex_exit]._startNode)]) exist = true;
            }
            if(exist){
                _graph->simplifySuperbubble(sw, _removedNodes, _isBubble, _saveState);
                return;
            }

            //_logFile << unitig._startNode << endl;

            #ifdef PRINT_DEBUG_SIMPLIFICATION
                _logFile << "\tSuperbubble: " << BiGraph::nodeToString(unitig._endNode) << " " << BiGraph::nodeToString(_unitigs[unitigIndex_exit]._startNode) << endl;
                _logFile << "\tSuperbubble: " << BiGraph::nodeToString(nodeIndex_toReverseDirection(unitig._endNode)) << " " << BiGraph::nodeToString(nodeIndex_toReverseDirection(_unitigs[unitigIndex_exit]._startNode)) << endl;
                //getchar();
            #endif

            //_logFile << "Found superbubble: " << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_exit]._startNode) << endl;
            simplifySuperbubble(unitig._index, unitigIndex_exit, _removedNodes, _isBubble, _saveState, sw);

        }

        //https://arxiv.org/pdf/1307.7925.pdf
        u_int32_t detectSuperbubble(u_int32_t unitigIndex_source, u_int64_t maxLength, vector<bool>& isBubble){

            //if(8236239 == _unitigs[unitigIndex_source]._startNode) _logFile << "Source: " << _unitigs[unitigIndex_source]._startNode << endl;
            //disconnectSubGraph(_unitigs[unitigIndex_source]._endNode);

            unordered_set<u_int32_t> isVisited;
            unordered_set<u_int32_t> seen;
            unordered_map<u_int32_t, u_int64_t> pathLength;
            vector<u_int32_t> queue;

            queue.push_back(unitigIndex_source);
            pathLength[unitigIndex_source] = 0;

            while(queue.size() > 0){
                u_int32_t v = queue[queue.size()-1];

                //if(8236239 == _unitigs[unitigIndex_source]._startNode) _logFile << "\t" << _unitigs[v]._startNode << " " << pathLength[v] << endl;
                //_logFile << "\tVisited: " << BiGraph::nodeIndex_to_nodeName(_unitigs[v]._startNode) << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[v]._endNode) << endl;
                queue.pop_back();

                //if(pathLength[v] > maxLength) continue;
                //_logFile << pathLength[v] << " " << _kminmerSize << endl;
                if(pathLength[v] > maxLength){
                    //_logFile << "exit 1" << endl;
                    return -1;
                    //continue;
                }

                isVisited.insert(v);
                if(seen.find(v) != seen.end()){
                    seen.erase(v);
                }

                vector<u_int32_t> successors;
                _graph->getSuccessors_unitig(v, 0, successors);

                //vector<u_int32_t> successors;
                //getSuccessors_unitig(v, 0, successors);
                if(successors.size() == 0){
                    //_logFile << "exit 2" << endl;
                    return -1; //abort tip
                }

                //for(u_int32_t nodeIndex : successors){
                //    _logFile << nodeIndex << " " << _graph->_nodeToUnitig[nodeIndex] << " " << isBubble[nodeIndex] << " " << isBubble[nodeIndex_toReverseDirection(nodeIndex)] << " " << _graph->_isNodeRemoved[nodeIndex] << " " << _graph->_isNodeRemoved[nodeIndex_toReverseDirection(nodeIndex)] << endl;
                //}
                std::sort(successors.begin(),successors.end(), [this](const u_int32_t& unitigIndex1, const u_int32_t& unitigIndex2) {
                    return _graph->_unitigs[unitigIndex1]._startNode < _graph->_unitigs[unitigIndex2]._startNode;
                });

                for(const u_int32_t& u : successors){
                    //u_int32_t u = node._index;

                    bool exist = false;
                    #pragma omp critical(superbubble)
                    {
                        if(isBubble[_unitigs[u]._startNode]) exist = true;
                        if(isBubble[nodeIndex_toReverseDirection(_unitigs[u]._startNode)]) exist = true;
                    }
                    if(exist){
                        //_logFile << "exit 3" << endl;
                        return -1;
                    }

                    //if(isBubble[_unitigs[u]._startNode]) return -1;
                    //if(isBubble[nodeIndex_toReverseDirection(_unitigs[u]._startNode)]) return -1;
                    if(u == unitigIndex_source) return -1; //cycle including s

                    if(isVisited.find(u) == isVisited.end()){
                        seen.insert(u);
                        long length = _unitigs[u]._length - _graph->_kminmerOverlapMean;
                        //_logFile << _unitigs[u]._length << " " << node._overlap << endl;
                        //if(length < 0) length = _unitigs[u]._length;
                        pathLength[u] = pathLength[v] + length;
                        //pathLength[u] = pathLength[v] + _unitigs[u]._nbNodes;
                    }
                    else{

                        //_logFile << "exit 4" << endl;
                        return -1; //Cycle within superbubble
                    }

                }

                for(const u_int32_t& u : successors){

                    //u_int32_t u = node._index;
                    //_logFile << "\t\tVisiting: " << BiGraph::nodeIndex_to_nodeName(_unitigs[u]._startNode) << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[u]._endNode) << endl;

                    vector<u_int32_t> predecessors;
                    _graph->getPredecessors_unitig(u, 0, predecessors);
                    bool allPredecessorsAreVisited = true;
                    for(u_int32_t p : predecessors){
                        if(isVisited.find(p) == isVisited.end()){
                            allPredecessorsAreVisited = false;
                            break;
                        }
                    }

                    if(allPredecessorsAreVisited){
                        //_logFile << "\t\tAll predecessors visited: " << BiGraph::nodeIndex_to_nodeName(_unitigs[u]._startNode) << endl;
                        queue.push_back(u);
                    }

                    //_logFile << "\t\t\tQueue size: " << queue.size() << " " << seen.size() << endl;
                    if(queue.size() == 1 && seen.size() == 1 && seen.find(queue[0]) != seen.end()){ //only one vertex t is left in S and no other vertex is seen 
                        u_int32_t t = queue[0];
                        vector<u_int32_t> successors_t;
                        _graph->getSuccessors_unitig(t, 0, successors_t);
                        if(std::find(successors_t.begin(), successors_t.end(), unitigIndex_source) == successors_t.end()){
                            return t;
                        }
                        else{
                            //_logFile << "exit 5" << endl;
                            return -1; // cycle including s
                        }

                    }


                }

            }

            //_logFile << "exit 6" << endl;
            return -1;
        }


        /*
        struct MostAbundantPath{
            u_int32_t _unitigIndex;
            u_int32_t _abundance;
            //u_int32_t _prevNodeIndex;
        };

        struct MostAbundantPath_Comparator {
            bool operator()(MostAbundantPath const& p1, MostAbundantPath const& p2){
                return p1._abundance < p2._abundance;
            }
        };
        */


        void simplifySuperbubble(u_int32_t source_unitigIndex, u_int32_t sink_unitigIndex, unordered_set<u_int32_t>& removedNodes, vector<bool>& isBubble, SaveState2& saveState, SuperbubbleWriter sw){

            vector<u_int32_t> unitigIndexes;
            _graph->collectNodes_betweenSourceSink_unitig(source_unitigIndex, sink_unitigIndex, unitigIndexes, 0, {}, true);
            //if(unitigIndexes.size() == 0) return;

            unordered_set<u_int32_t> keepNodes;
            vector<u_int32_t> nodes;
            //vector<u_int32_t> removedNodes;
            //Bubble bubble = {_unitigs[source_unitigIndex]._endNode, _unitigs[source_unitigIndex]._startNode, {}};
            //Bubble bubbleRC = {nodeIndex_toReverseDirection(_unitigs[source_unitigIndex]._startNode), nodeIndex_toReverseDirection(_unitigs[source_unitigIndex]._endNode), {}};;

            /*
            unordered_set<u_int32_t> isVisited;
            




        
            bool found = false;
            priority_queue< MostAbundantPath, vector <MostAbundantPath> , MostAbundantPath_Comparator> pq;
            unordered_map<u_int32_t, u_int32_t> dist;
            unordered_map<u_int32_t, u_int32_t> prev;

            pq.push({source_unitigIndex, 0});
            dist[source_unitigIndex] = 0;
            prev[source_unitigIndex] = 0;
        
            while(!pq.empty()){
                
                int u = pq.top()._unitigIndex;
                pq.pop();
                
                //_logFile << u << endl;
                //_logFile << u << endl;
                //if(_unitigs[u]._startNode == -1) continue;
                
                //if(dests.find(u) != dests.end() || dests.find(unitigIndex_toReverseDirection(u)) != dests.end()){
                //    dests.erase(u);
                //}
                //_logFile << dests.size() << " " << prev.size() << endl;
                //if(dests.size() == 0) break;

                //if(pass == 60){
                //    for(u_int32_t nodeIndex : _unitigs[u]._nodes){
                //        file_scc << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "red" << endl;
                //    }
                //}


                //_logFile << u << " " << pq.size() << endl;
                vector<u_int32_t> successors;
                getSuccessors_unitig(u, 0, successors);
                //vector<u_int32_t> predecessors;
                //getPredecessors_unitig(u, 0, predecessors);

                //vector<u_int32_t> neighbors;
                //neighbors.insert(neighbors.end(), successors.begin(), successors.end());
                //neighbors.insert(neighbors.end(), predecessors.begin(), predecessors.end());


                for(u_int32_t v : successors){

                    u_int32_t weight = _unitigs[v]._abundance;
                    
                    if(dist.find(v) == dist.end()){
                        dist[v] = 0;
                    }


                    if(dist[v] < dist[u] + weight){
                        
                        prev[v] = u;

                        if(v == sink_unitigIndex){
                            found = true;
                            break;
                        }
                    
                        //if(v == sink_unitigIndex || v == sink_unitigIndex_rev){
                        //    found_unitigIndex = v;
                        //    found = true;
                        //    break;
                        //}

                        dist[v] = dist[u] + weight;
                        pq.push({v, dist[v]});
                    }
                }

                if(found) break;

            }

            if(!found){
                _logFile << "Not found path in superbubble with cycle" << endl;
                exit(1);
            }
            
            vector<u_int32_t> path;

            u_int32_t n = sink_unitigIndex;
            while(n != source_unitigIndex){

                if(n == sink_unitigIndex){
                    path.push_back(n);
                }
                else{
                    path.push_back(n);
                }

                n = prev[n];
            }
            path.push_back(source_unitigIndex);
            */


            //Choose one path in the superbubble
            u_int32_t unitigIndex = source_unitigIndex;
            vector<u_int32_t> path = {source_unitigIndex};
            while(true){
                vector<u_int32_t> successors;
                _graph->getSuccessors_unitig(unitigIndex, 0, successors);

                //_logFile << _unitigs[unitigIndex]._startNode << endl;

                vector<u_int32_t> maxAbNodes;
                float maxAbundance = 0;
                u_int32_t maxV = -1;
                for(u_int32_t v : successors){
                    //_logFile << "\t" << _unitigs[v]._abundance << endl;
                    //for(u_int32_t nodeIndex : _unitigs[v]._nodes){
                    //    _logFile << nodeIndex << " ";
                    //}
                    //_logFile << endl;
                    if(_unitigs[v]._abundance == maxAbundance){
                        maxAbNodes.push_back(v);
                    }
                    else if(_unitigs[v]._abundance > maxAbundance){
                        maxAbundance = _unitigs[v]._abundance;
                        maxAbNodes.clear();
                        maxAbNodes.push_back(v);
                        //maxV = v;
                    }
                }

                if(maxAbNodes.size() == 1){
                    path.push_back(maxAbNodes[0]);
                    unitigIndex = maxAbNodes[0];
                }
                else{

                    
                    //_logFile << "----" << endl;
                    vector<BubbleSide> bubbleSides;
                    for(u_int32_t unitigIndex : maxAbNodes){

                        
                        //_logFile << "equal: " << unitigIndex << " " << _unitigs[unitigIndex]._quality << " " << _unitigs[unitigIndex]._nbNodes << " " << _unitigs[unitigIndex]._abundance << endl;

                        //_logFile << "\t-" << endl;
                        u_int64_t sum = 0;
                        for(u_int32_t nodeIndex : _unitigs[unitigIndex]._nodes){
                            //_logFile << "\t" << nodeIndex << ": " << _graph->_graphSuccessors->_nodeDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex)]._quality << endl;
                            sum += nodeIndex;
                        }
                        
                        bubbleSides.push_back({unitigIndex, _unitigs[unitigIndex]._nbNodes, _unitigs[unitigIndex]._startNode}); //_unitigs[unitigIndex]._quality
                    }


                    std::sort(bubbleSides.begin(), bubbleSides.end(), BubbleSideComparator);
                    path.push_back(bubbleSides[0]._unitigIndex);
                    //_logFile << "Keep: " << bubbleSides[0]._unitigIndex << endl;

                    unitigIndex = bubbleSides[0]._unitigIndex;

                    //_logFile << "Keep node: " << _unitigs[unitigIndex]._startNode << endl;
                    //_logFile << _unitigs[unitigIndex]._nbNodes;
                    //getchar();
                }
                
                //unitigIndex = maxV;

                if(unitigIndex == sink_unitigIndex) break;
            }
            
            
            /*
            bool print_superbubble = false;
            for(u_int32_t unitigIndex : unitigIndexes){
                getUnitigNodes(_unitigs[unitigIndex], nodes);
                for(u_int32_t nodeIndex : nodes){
                    if(BiGraph::nodeIndex_to_nodeName(nodeIndex) == 510153){
                        print_superbubble = true;
                        _logFile << "------" << endl;
                        _logFile << path.size() << endl;
                    }
                    if(BiGraph::nodeIndex_to_nodeName(nodeIndex) == 3300236){
                        print_superbubble = true;
                        _logFile << "------" << endl;
                        _logFile << path.size() << endl;
                    }
                    
                }
            }
            */

            //shortestPath_unitig(source_unitigIndex, sink_unitigIndex, path, false, false);
            for(u_int32_t unitigIndex : path){
                _graph->getUnitigNodes(_unitigs[unitigIndex], nodes);
                for(u_int32_t nodeIndex : nodes){
                    //_logFile << "\tKeep nodes: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                    keepNodes.insert(nodeIndex);
                    //keepNodes.insert(nodeIndex_toReverseDirection(nodeIndex));
                    //if(print_superbubble){
                    //_logFile << "keep node: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                    //}
                    //keepNodes.insert(nodeIndex_toReverseDirection(nodeIndex));
                }
            }
    
            //_logFile << "simp bubble " << keepNodes.size() << " " << unitigIndexes.size() << endl;

            //_logFile << "OMG: " << processIndex << endl;
            //SuperbubbleWriter sw = {processIndex, {}};

            for(u_int32_t unitigIndex : unitigIndexes){
                _graph->getUnitigNodes(_unitigs[unitigIndex], nodes);
                for(u_int32_t nodeIndex : nodes){
                    /*
                    isBubble[nodeIndex] = true;
                    isBubble[nodeIndex_toReverseDirection(nodeIndex)] = true;
                    */
                    //_isBubble[nodeIndex] = true;
                    //_isBubble[nodeIndex_toReverseDirection(nodeIndex)] = true;

                    //saveState._isBubble.push_back(BiGraph::nodeIndex_to_nodeName(nodeIndex));
                    //if(print_superbubble){
                    //    _logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                    //}



                    if(keepNodes.find(nodeIndex) != keepNodes.end()) continue;
                    if(keepNodes.find(_graph->nodeIndex_toReverseDirection(nodeIndex)) != keepNodes.end()) continue;

                    sw._nodeIndexRemoved.push_back(nodeIndex);
                    sw._nodeIndexRemoved.push_back(_graph->nodeIndex_toReverseDirection(nodeIndex));



                    //sw._nodeIndexRemoved.push_back(nodeIndex_toReverseDirection(nodeIndex));
                    //if(keepNodes.find(nodeIndex_toReverseDirection(nodeIndex)) != keepNodes.end()) continue;
                    //removedNodes.push_back(nodeIndex);
                    //bubble._nodes.push_back({nodeIndex, _graph->getNodeUnitigAbundance(nodeIndex)});
                    //bubbleRC._nodes.push_back({nodeIndex_toReverseDirection(nodeIndex), _graph->getNodeUnitigAbundance(nodeIndex_toReverseDirection(nodeIndex)) });
                    //_logFile << "\tRemove nodes: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                }
            }

            /*
            #pragma omp critical(superbubble)
            {
                bool isHere = false;

                for(u_int32_t unitigIndex : unitigIndexes){
                    _graph->getUnitigNodes(_unitigs[unitigIndex], nodes);
                    for(u_int32_t nodeIndex : nodes){
                        if(BiGraph::nodeIndex_to_nodeName(nodeIndex) == 17360) isHere = true;
                    }
                }

                if(isHere){
                    for(u_int32_t nodeIndex : keepNodes){
                        _logFile << "Keep: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                    }
                    for(u_int32_t unitigIndex : unitigIndexes){
                        _graph->getUnitigNodes(_unitigs[unitigIndex], nodes);
                        for(u_int32_t nodeIndex : nodes){
                            if(keepNodes.find(nodeIndex) != keepNodes.end()) continue;
                            _logFile << unitigIndex << " " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                            if(BiGraph::nodeIndex_to_nodeName(nodeIndex) == 17360) isHere = true;
                        }
                    }

                }
            }
            */


            _graph->simplifySuperbubble(sw, removedNodes, isBubble, saveState);
            /*
            for(u_int32_t unitigIndex : unitigIndexes){
                _graph->getUnitigNodes(_unitigs[unitigIndex], nodes);
                for(u_int32_t nodeIndex : nodes){
                    isBubble[nodeIndex] = true;
                    isBubble[nodeIndex_toReverseDirection(nodeIndex)] = true;
                    //_isBubble[nodeIndex] = true;
                    //_isBubble[nodeIndex_toReverseDirection(nodeIndex)] = true;

                    //saveState._isBubble.push_back(BiGraph::nodeIndex_to_nodeName(nodeIndex));
                    //if(print_superbubble){
                    //    _logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                    //}

                    if(keepNodes.find(nodeIndex) != keepNodes.end()) continue;
                    //if(keepNodes.find(nodeIndex_toReverseDirection(nodeIndex)) != keepNodes.end()) continue;
                    //removedNodes.push_back(nodeIndex);
                    bubble._nodes.push_back({nodeIndex, _graph->getNodeUnitigAbundance(nodeIndex)});
                    //bubbleRC._nodes.push_back({nodeIndex_toReverseDirection(nodeIndex), _graph->getNodeUnitigAbundance(nodeIndex_toReverseDirection(nodeIndex)) });
                    //_logFile << "\tRemove nodes: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                }
            }
               
            //if(print_superbubble){
            //    getchar();
            //}

            //std::reverse(bubbleRC._nodes.begin(), bubbleRC._nodes.end());

            
            bool cancelBubble = false;

            if(bubble._nodes.size() == 0){ 
                _logFile << "1" << endl;
                _logFile << path.size() << " " << unitigIndexes.size() << endl;
                //_logFile << source_unitigIndex << endl;
                //_logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[source_unitigIndex]._startNode) << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[sink_unitigIndex]._startNode) << endl;
                //getchar();
                for(u_int32_t nodeIndex : keepNodes){
                    if(_graph->nodeIndex_to_unitigIndex(nodeIndex) == source_unitigIndex) continue;
                    if(_graph->nodeIndex_to_unitigIndex(nodeIndex) == sink_unitigIndex) continue;
                    removedNodes.insert(nodeIndex);
                    removedNodes.insert(nodeIndex_toReverseDirection(nodeIndex));
                    saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex));
                }

                //return;

                cancelBubble = true;
            }

            for(NodeAbundance& nodeIndex : bubble._nodes){ //removed nodes
                //_logFile << "1" << endl;
                //_isNodeValid2.erase(nodeIndex);
                removedNodes.insert(nodeIndex._nodeIndex);
                removedNodes.insert(nodeIndex_toReverseDirection(nodeIndex._nodeIndex));
                saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex._nodeIndex));
                //_isNodeValid2.erase(nodeIndex_toReverseDirection(nodeIndex));
            }
            */

            //sdfsdqfsdfsfdgsfdgsdfgsdf


        }

    };

    void simplifySuperbubble(SuperbubbleWriter swNext, unordered_set<u_int32_t>& removedNodes, vector<bool>& isBubble, SaveState2& saveState){


        #pragma omp critical(superbubble)
        {


            //getchar();
            _readWriterQueue.push(swNext);

            while(!_readWriterQueue.empty()){

                const SuperbubbleWriter& sw = _readWriterQueue.top();

                //_logFile << sw._readIndex << " " << _nextReadIndexWriter << endl;
                if(sw._readIndex == _nextReadIndexWriter){


                    bool isValid = true;
                    for(u_int32_t nodeIndex : sw._nodeIndexRemoved){
                        if(isBubble[nodeIndex] || isBubble[nodeIndex_toReverseDirection(nodeIndex)]){
                            isValid = false;
                            break;
                        }
                    }

                    if(isValid){

                        /*
                        if(!__debug_lala){
                            if(sw._nodeIndexRemoved.size() > 0){
                                _logFile << "\t" << sw._readIndex << endl;
                                _logFile << "\t\tCrush bubble: " << sw._nodeIndexRemoved[0] << endl;// << " " << std::fixed << " " << getChecksumGlobal_nbUtg(removedNodes) << endl;
                                unordered_set<u_int32_t> utgs;
                                for(u_int32_t nodeIndex : sw._nodeIndexRemoved){
                                    utgs.insert(nodeIndex_to_unitigIndex(nodeIndex));
                                //    _logFile << "\t\t" << nodeIndex << endl;
                                }

                                _logFile << "\t\t" << utgs.size() << endl;
                                
                                //for(u_int32_t unitigIndex: utgs){
                                    //_logFile << "\t\t" << unitigIndex << " " << _unitigs[unitigIndex]._nbNodes << endl;
                                //}
                                //getchar();
                            }
                        }
                        */
                        
                        
                        
                        
                        
                        for(u_int32_t nodeIndex : sw._nodeIndexRemoved){
                            //if(nodeIndex == 11355916 || nodeIndex == 11725329) getchar();
                            isBubble[nodeIndex] = true;
                            isBubble[nodeIndex_toReverseDirection(nodeIndex)] = true;

                            removedNodes.insert(nodeIndex);
                            removedNodes.insert(nodeIndex_toReverseDirection(nodeIndex));
                            saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex));
                            _superbubble_checksum += BiGraph::nodeIndex_to_nodeName(nodeIndex);

                            //_logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                        }

                        //if(sw._nodeIndexRemoved.size() > 0) _logFile << sw._readIndex << ": " << _superbubble_checksum << endl;
                        //if(1122 == sw._readIndex) getchar();
                    }
                    
                    /*
                    if(!__debug_lala){
                        //if(sw._nodeIndexRemoved.size() > 0 && 897358 == sw._nodeIndexRemoved[0]) getchar();
                        
                        if(_nextReadIndexWriter % 100000 == 0){
                        //    _logFile << _nextReadIndexWriter << endl;
                        //    double lala = getChecksumGlobal_nbUtg(removedNodes);
                            _logFile << "Bubble: " << std::fixed << " " << getChecksumGlobal_nbUtg(removedNodes) << endl;
                                //_logFile << getNodeUnitigAbundance(894141) << endl;
                                //getchar();
                               // getchar();
                        //    _logFile << "\t" << lala << endl;
                        //    if(lala < 828230)  getchar();
                        }
                        //_logFile << getChecksumGlobal_nbUtg(removedNodes) << endl;
                    }
                    */
                    

                    _readWriterQueue.pop();
                    _nextReadIndexWriter += 1;
                }
                else{
                    break;
                }
            }

        }
    }


    /*
                bool isValid = true;
                for(u_int32_t unitigIndex : unitigIndexes){
                    for(u_int32_t nodeIndex : nodes){
                        if(isBubble[nodeIndex]){
                            isValid = false;
                            break;
                        }
                    }
                    if(!isValid) break;
                }


                if(isValid){

                    //Mark all nodes as bubble
                    for(u_int32_t unitigIndex : unitigIndexes){
                        _graph->getUnitigNodes(_unitigs[unitigIndex], nodes);
                        for(u_int32_t nodeIndex : nodes){
                            isBubble[nodeIndex] = true;
                            isBubble[nodeIndex_toReverseDirection(nodeIndex)] = true;
                            //_isBubble[nodeIndex] = true;
                            //_isBubble[nodeIndex_toReverseDirection(nodeIndex)] = true;

                            //saveState._isBubble.push_back(BiGraph::nodeIndex_to_nodeName(nodeIndex));
                            //if(print_superbubble){
                            //    _logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                            //}

                            if(keepNodes.find(nodeIndex) != keepNodes.end()) continue;
                            //if(keepNodes.find(nodeIndex_toReverseDirection(nodeIndex)) != keepNodes.end()) continue;
                            //removedNodes.push_back(nodeIndex);
                            bubble._nodes.push_back({nodeIndex, _graph->getNodeUnitigAbundance(nodeIndex)});
                            //bubbleRC._nodes.push_back({nodeIndex_toReverseDirection(nodeIndex), _graph->getNodeUnitigAbundance(nodeIndex_toReverseDirection(nodeIndex)) });
                            //_logFile << "\tRemove nodes: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                        }
                    }
                            
                    //if(print_superbubble){
                    //    getchar();
                    //}

                    //std::reverse(bubbleRC._nodes.begin(), bubbleRC._nodes.end());

                    bool cancelBubble = false;

                    if(bubble._nodes.size() == 0){ 
                        _logFile << "1" << endl;
                        _logFile << path.size() << " " << unitigIndexes.size() << endl;
                        //_logFile << source_unitigIndex << endl;
                        //_logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[source_unitigIndex]._startNode) << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[sink_unitigIndex]._startNode) << endl;
                        //getchar();
                        for(u_int32_t nodeIndex : keepNodes){
                            if(_graph->nodeIndex_to_unitigIndex(nodeIndex) == source_unitigIndex) continue;
                            if(_graph->nodeIndex_to_unitigIndex(nodeIndex) == sink_unitigIndex) continue;
                            removedNodes.insert(nodeIndex);
                            removedNodes.insert(nodeIndex_toReverseDirection(nodeIndex));
                            saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex));
                        }

                        //return;

                        cancelBubble = true;
                    }

                    for(NodeAbundance& nodeIndex : bubble._nodes){ //removed nodes
                        //_logFile << "1" << endl;
                        //_isNodeValid2.erase(nodeIndex);
                        removedNodes.insert(nodeIndex._nodeIndex);
                        removedNodes.insert(nodeIndex_toReverseDirection(nodeIndex._nodeIndex));
                        saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex._nodeIndex));
                        //_isNodeValid2.erase(nodeIndex_toReverseDirection(nodeIndex));
                    }


                }
            }
        }
        */




    u_int32_t _nextUnitigIndex;


    /*
    u_int32_t detectSuperbubble_withCycle(u_int32_t unitigIndex_source, u_int64_t maxLength, vector<bool>& isBubble){

        




        bool forward = true;

        disconnectSubGraph(unitigIndex_source, maxLength, true);

		vector<vector<u_int32_t>> components;
        getStronglyConnectedComponents(unitigIndex_source, forward, components);

		unordered_map<u_int32_t, vector<u_int32_t>> unitigToSccs;
		for(vector<u_int32_t>& component : components){
			for(u_int32_t unitigIndex : component){
				unitigToSccs[unitigIndex] = component;
			}
			
		}


        unordered_set<u_int32_t> isVisited;
        unordered_set<u_int32_t> seen;
        //unordered_map<u_int32_t, u_int64_t> pathLength;
        vector<u_int32_t> queue;

        queue.push_back(unitigIndex_source);
        //pathLength[unitigIndex_source] = 0;

        while(queue.size() > 0){
            u_int32_t v = queue[queue.size()-1];
            //_logFile << "\tVisited: " << BiGraph::nodeIndex_to_nodeName(_unitigs[v]._startNode) << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[v]._endNode) << endl;
            queue.pop_back();

            //if(pathLength[v] > maxLength) continue;

            isVisited.insert(v);
            if(seen.find(v) != seen.end()){
                seen.erase(v);
            }

            const vector<u_int32_t>& scc = unitigToSccs[v];

            unordered_set<u_int32_t> successors;
            getSccSuccessors(scc, forward, successors);
            if(successors.size() == 0) return -1; //abort tip

            for(u_int32_t u : successors){
                if(isBubble[_unitigs[u]._startNode]) return -1;
                if(isBubble[nodeIndex_toReverseDirection(_unitigs[u]._startNode)]) return -1;
                if(u == unitigIndex_source) return -1; //cycle including s

                if(isVisited.find(u) == isVisited.end()){
                    seen.insert(u);
                    //pathLength[u] = pathLength[v] + _unitigs[u]._length;
                }

            }

            for(u_int32_t u : successors){

                //_logFile << "\t\tVisiting: " << BiGraph::nodeIndex_to_nodeName(_unitigs[u]._startNode) << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[u]._endNode) << endl;

                const vector<u_int32_t>& scc_u = unitigToSccs[u];

                unordered_set<u_int32_t> predecessors;
                getSccPredecessors(scc_u, forward, predecessors);
                bool allPredecessorsAreVisited = true;
                for(u_int32_t p : predecessors){
                    if(isVisited.find(p) == isVisited.end()){
                        allPredecessorsAreVisited = false;
                        break;
                    }
                }

                if(allPredecessorsAreVisited){
                    //_logFile << "\t\tAll predecessors visited: " << BiGraph::nodeIndex_to_nodeName(_unitigs[u]._startNode) << endl;
                    queue.push_back(u);
                }

                //_logFile << "\t\t\tQueue size: " << queue.size() << " " << seen.size() << endl;
                if(queue.size() == 1 && seen.size() == 1 && seen.find(queue[0]) != seen.end()){ //only one vertex t is left in S and no other vertex is seen 
                    u_int32_t t = queue[0];

                    const vector<u_int32_t>& scc_t = unitigToSccs[t];

                    unordered_set<u_int32_t> successors_t;
                    getSccSuccessors(scc_t, forward, successors_t);
                    if(std::find(successors_t.begin(), successors_t.end(), unitigIndex_source) == successors_t.end()){
                        return t;
                    }
                    else{
                        return -1; // cycle including s
                    }

                }


            }

        }

        return -1;
    }

	void getSccSuccessors(const vector<u_int32_t>& scc, bool forward, unordered_set<u_int32_t>& sccSuccessors){

		sccSuccessors.clear();

		for(u_int32_t vv : scc){
			
			vector<u_int32_t> successors;
			if(forward){
				getSuccessors_unitig(vv, 0, successors);
			}
			else{
				getPredecessors_unitig(vv, 0, successors);
			}
			
			for(u_int32_t u : successors){
				if(std::find(scc.begin(), scc.end(), u) != scc.end()) continue;
				sccSuccessors.insert(u);
			}

		}

	}

	void getSccPredecessors(const vector<u_int32_t>& scc, bool forward, unordered_set<u_int32_t>& sccSuccessors){

		sccSuccessors.clear();

		for(u_int32_t vv : scc){

			vector<u_int32_t> successors;
			if(forward){
				getPredecessors_unitig(vv, 0, successors);
			}
			else{
				getSuccessors_unitig(vv, 0, successors);
			}
			
			for(u_int32_t u : successors){
				if(std::find(scc.begin(), scc.end(), u) != scc.end()) continue;
				sccSuccessors.insert(u);
			}

		}

	}
    */

    u_int64_t bubble(float maxLength, vector<bool>& isBubble, SaveState2& saveState, bool useReadpathSubgraph, GraphSimplify* graphMain){

        _nextReadIndexWriter = 0;
	    _readWriterQueue = priority_queue<SuperbubbleWriter, vector<SuperbubbleWriter> , SuperbubbleWriter_Comparator>();

        
        unordered_set<u_int32_t> removedNodes;

        u_int64_t nbBubblesRemoved = 0;
        //vector<u_int32_t> removedUnitigs;

        //vector<bool> isVisited(_graphSuccessors->_nbNodes, false);
        //vector<u_int32_t> unitigNodes;
        //bool dummy = false;


		//unordered_set<u_int32_t> writtenUnitigs;

        vector<UnitigTip> unitigTips;

        if(_unitigIndexToClean.size() == 0){
            for(const Unitig& u: _unitigs){
                if(u._startNode == -1) continue;
                //if(u._index % 2 == 1) continue;
                //if(u._startNode % 2 != 0) continue;

                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._startNode)) != writtenUnitigs.end()) continue;
                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._endNode)) != writtenUnitigs.end()) continue;

                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._startNode));
                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._endNode));
                
                unitigTips.push_back({u._index, u._startNode, u._length});
            }

        }
        else{

            for(u_int32_t unitigIndex : _unitigIndexToClean){
                const Unitig& u = _unitigs[unitigIndex];
                if(u._startNode == -1) continue;
                //if(u._index % 2 == 1) continue;
    
                //if(u._length > maxLength) continue;
                unitigTips.push_back({u._index, u._startNode, u._length});
            }
        }
        
        //std::sort(unitigTips.begin(), unitigTips.end(), UnitigTipComparator_ByLength_Reverse);
        
        //srand(time(NULL));
        //std::random_shuffle(unitigTips.begin(), unitigTips.end());
        
        std::sort(unitigTips.begin(), unitigTips.end(), UnitigTipComparator_ByLength);

        BubbleFunctor functor(this, removedNodes, isBubble, maxLength, saveState);
        size_t i = 0;

        #pragma omp parallel num_threads(_nbCores)
        {

            BubbleFunctor functorSub(functor);
            SuperbubbleWriter sw;

            while(true){

                u_int32_t unitigIndex = -1;
                bool isEof = false;

                #pragma omp critical
                {
                    
                    if(i >= unitigTips.size()){
                        isEof = true;
                    }
                    else{
                        unitigIndex = unitigTips[i]._unitigIndex;
                        //_logFile << nodeIndex << endl;
                    }

                    //_logFile << unitigIndex << " " << _unitigs[unitigIndex]._startNode << endl;
                    //_logFile << _unitigs[unitigIndex]._length << " " << _unitigs[unitigIndex]._startNode << endl;
                    //getchar();
                    //_logFile << i << endl;
                    sw = {i, unitigIndex, {}};
                    i += 1;
                }

                if(isEof) break;
                functorSub(sw);

            }
            

        }


        for(const UnitigTip& unitigTip : unitigTips){


            const Unitig& unitig = _unitigs[unitigTip._unitigIndex];




        }

        unordered_set<u_int32_t> removedUnitigs;
        for(u_int32_t nodeIndex : removedNodes){
            removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
        }
        removeUnitigs(removedUnitigs);
        for(u_int32_t nodeIndex : removedNodes){

            _isNodeRemoved[nodeIndex] = true;
            //_isNodeValid2.erase(nodeIndex);
            //file_debug << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "green" << endl;
            //if(395881 == BiGraph::nodeIndex_to_nodeName(nodeIndex)){
              //      _logFile << "lala6" << endl;
               //     getchar();
                //}
            //_removedFrom[nodeIndex] = 3;
        }

        return removedUnitigs.size();
    }

   
	class BubbleFunctor {

		public:

		GraphSimplify* _graph;
        unordered_set<u_int32_t>& _removedNodes;
        vector<bool>& _isVisited;
        vector<Unitig>& _unitigs;
        SaveState2& _saveState;
        u_int64_t _maxLength;

		BubbleFunctor(GraphSimplify* graph, unordered_set<u_int32_t>& removedNodes, vector<bool>& isVisited, u_int64_t maxLength, SaveState2& saveState) : _graph(graph), _removedNodes(removedNodes), _isVisited(isVisited), _unitigs(graph->_unitigs), _saveState(saveState){
            _maxLength = maxLength;
        }

		BubbleFunctor(const BubbleFunctor& copy) : _graph(copy._graph), _removedNodes(copy._removedNodes), _isVisited(copy._isVisited), _unitigs(copy._graph->_unitigs), _saveState(copy._saveState){
            _maxLength = copy._maxLength;
		}

		~BubbleFunctor(){
		}

		void operator () (SuperbubbleWriter sw) {
            
            u_int32_t unitigIndex = sw._unitigIndex_source;
            const Unitig& unitig = _graph->_unitigs[unitigIndex];

        //for(Unitig& unitig : _unitigs){
            //if(unitig._startNode == -1) continue;
            //_logFile << "-------------" << endl;

            //u_int32_t startNodeName = _graphSuccessors->nodeIndex_to_nodeName(unitig._startNode, dummy);
            //u_int32_t visitedNodeIndex = unitig._startNode;

            bool exist = false;
            #pragma omp critical(superbubble)
            {
                if(_isVisited[unitig._startNode] || _isVisited[nodeIndex_toReverseDirection(unitig._startNode)]) exist = true;
            }
            if(exist){
                _graph->simplifySuperbubble(sw, _removedNodes, _isVisited, _saveState);
                return;
            }

            //if(_isBubble[unitig._startNode]) continue;
            //if(_isBubble[nodeIndex_toReverseDirection(unitig._startNode)]) continue;
            

            //_logFile << "--------------" << endl;
            //if(isVisited[unitig._startNode]) continue;
            //if(isVisited[unitig._endNode]) continue;

            //isVisited[unitig._startNode] = true;
            //isVisited[unitig._endNode] = true;


            Unitig& utg_1 = _unitigs[unitig._index];

            //getNeighbors(utg_1._endNode, orientation, neighbors);
            vector<u_int32_t> neighbors_utg1;
            _graph->getSuccessors(utg_1._endNode, 0, neighbors_utg1);
            

            //_logFile << _graphSuccessors->nodeIndex_to_nodeName(utg_1._endNode, dummy) << " " << neighbors.size() << endl;

            //else if(_graphSuccessors->nodeIndex_to_nodeName(unitig._startNode, dummy) == 6734){
            //    _logFile << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << endl;
                //_logFile << _graphSuccessors->nodeIndex_to_nodeName(utg_1._endNode, dummy) << " " << neighbors.size() << endl;
            //}
            
            if(neighbors_utg1.size() <= 1 || neighbors_utg1.size() > 5){
                _graph->simplifySuperbubble(sw, _removedNodes, _isVisited, _saveState);
                return;
            }

            std::sort(neighbors_utg1.begin(),neighbors_utg1.end(), [this](const u_int32_t& nodeIndex1, const u_int32_t& nodeIndex2) {
                return _graph->nodeIndex_to_unitig(nodeIndex1)._startNode < _graph->nodeIndex_to_unitig(nodeIndex2)._startNode;
            });

                /*
            if(useReadpathSubgraph){

                extractReadpathSubgraph(utg_1._index);
                for(u_int32_t nodeIndex : utg_1._nodes){
                    if(BiGraph::nodeIndex_to_nodeName(nodeIndex) == 2217129){


                        unordered_set<u_int32_t> validNodes;
                        for (auto& nodeIndex : _isNodeValid2){
                            u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
                            if(_debug_groundTruthNodeNames.find(nodeName) == _debug_groundTruthNodeNames.end()) continue;
                            validNodes.insert(nodeName);
                        }
                        string outputFilename = _outputDir + "/minimizer_graph_sub.gfa";
                        GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, outputFilename, validNodes, _isEdgeRemoved, _graphSuccessors);


                        _logFile << "Readpath subgraph: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;


                        getchar();
                    }
                    
                    if(BiGraph::nodeIndex_to_nodeName(nodeIndex) == 2520263){

                        extractReadpathSubgraph(utg_1._index);

                        unordered_set<u_int32_t> validNodes;
                        for (auto& nodeIndex : _isNodeValid2){
                            u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
                            if(_debug_groundTruthNodeNames.find(nodeName) == _debug_groundTruthNodeNames.end()) continue;
                            validNodes.insert(nodeName);
                        }
                        string outputFilename = _outputDir + "/minimizer_graph_sub.gfa";
                        GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, outputFilename, validNodes, _isEdgeRemoved, _graphSuccessors);


                        _logFile << "Readpath subgraph: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                        getchar();
                    }
                    
                }
            }
            */

            bool isBubble = false;

            for(size_t i=0; i<neighbors_utg1.size(); i++) {
                

                Unitig& utg_2 = _graph->nodeIndex_to_unitig(neighbors_utg1[i]); // _unitigs[_nodeToUnitig[neighbors_utg1[i]]];
                //visitedNodeIndex = _graphSuccessors->nodeIndex_to_nodeName(utg_2._startNode, dummy);
                //if(_isBubble[utg_2._startNode]) continue;
                //if(_isBubble[nodeIndex_toReverseDirection(utg_2._startNode)]) continue;
                //if(_isVisited[utg_2._startNode] || _isVisited[nodeIndex_toReverseDirection(utg_2._startNode)]) continue;
                //isVisited[startNodeName] = true;



                for(size_t j=i+1; j<neighbors_utg1.size(); j++){
                    Unitig& utg_3 = _graph->nodeIndex_to_unitig(neighbors_utg1[j]); //_unitigs[_nodeToUnitig[neighbors_utg1[j]]];
                    //startNodeName = _graphSuccessors->nodeIndex_to_nodeName(utg_3._startNode, dummy);
                    //if(_isVisited[utg_3._startNode] || _isVisited[nodeIndex_toReverseDirection(utg_3._startNode)]) continue;
                    //if(_isBubble[utg_3._startNode]) continue;
                    //if(_isBubble[nodeIndex_toReverseDirection(utg_3._startNode)]) continue;
                    //isVisited[startNodeName] = true;


                    vector<u_int32_t> neighbors_utg2;
                    _graph->getPredecessors(utg_2._startNode, 0, neighbors_utg2);
                    if(neighbors_utg2.size() != 1) continue;
                    _graph->getSuccessors(utg_2._endNode, 0, neighbors_utg2);
                    if(neighbors_utg2.size() != 1) continue;
                    
                    Unitig& utg_4 = _graph->nodeIndex_to_unitig(neighbors_utg2[0]); //_unitigs[_nodeToUnitig[neighbors_utg2[0]]];

                    //startNodeName = _graphSuccessors->nodeIndex_to_nodeName(utg_4._startNode, dummy);
                    //if(_isVisited[utg_4._startNode] || _isVisited[nodeIndex_toReverseDirection(utg_4._startNode)]) continue;
                    //if(_isBubble[utg_4._startNode]) continue;
                    //if(_isBubble[nodeIndex_toReverseDirection(utg_4._startNode)]) continue;
                    //isVisited[startNodeName] = true;


                    vector<u_int32_t> neighbors_utg3;
                    _graph->getPredecessors(utg_3._startNode, 0, neighbors_utg3);
                    if(neighbors_utg3.size() != 1) continue;
                    _graph->getSuccessors(utg_3._endNode, 0, neighbors_utg3);
                    if(neighbors_utg3.size() != 1) continue;

                    if(_graph->nodeIndex_to_unitigIndex(neighbors_utg3[0]) != utg_4._index) continue;
                    if(BiGraph::nodeIndex_to_nodeName(utg_1._endNode) == BiGraph::nodeIndex_to_nodeName(utg_4._startNode)) continue; //Repeated unitig with a cycle on on side
                    if(utg_2._length > _maxLength || utg_3._length > _maxLength) continue;
                    //if(utg_2._nbNodes > maxLength || utg_3._nbNodes > maxLength) continue;

                    #ifdef PRINT_DEBUG_SIMPLIFICATION
                        _logFile << "\tBubble: " << _graphSuccessors->nodeIndex_to_nodeName(utg_1._endNode, dummy) << " " << _graphSuccessors->nodeIndex_to_nodeName(utg_2._startNode, dummy) << " " << _graphSuccessors->nodeIndex_to_nodeName(utg_3._startNode, dummy) << " " << _graphSuccessors->nodeIndex_to_nodeName(utg_4._startNode, dummy) << " " << utg_2._length << " " << utg_3._length << endl;
                    #endif

                    //#pragma omp critical(bubble)
                    //{

                    //bool isValid = true;
                    //if(_isVisited[utg_1._startNode] || _isVisited[utg_2._startNode] || _isVisited[utg_3._startNode] || _isVisited[utg_4._startNode]) isValid = false;


                    //if(isValid){

                    /*
                    //Memorise bubble unitigs
                    //_graph->getUnitigNodes(utg_3, unitigNodes);
                    for(u_int32_t node : utg_3._nodes){
                        _isVisited[node] = true;
                        _isVisited[nodeIndex_toReverseDirection(node)] = true;
                        //_saveState._isBubble.push_back(BiGraph::nodeIndex_to_nodeName(node));

                        //if(BiGraph::nodeIndex_to_nodeName(node) == 510153){
                        //    _logFile << "bubulu (1): " << BiGraph::nodeIndex_to_nodeName(utg_2._nodes[0]) << endl;
                        //    getchar();
                        //}
                    }
                    //getUnitigNodes(utg_2, unitigNodes);
                    for(u_int32_t node : utg_2._nodes){
                        _isVisited[node] = true;
                        _isVisited[nodeIndex_toReverseDirection(node)] = true;
                        //_saveState._isBubble.push_back(BiGraph::nodeIndex_to_nodeName(node));

                        //if(BiGraph::nodeIndex_to_nodeName(node) == 510153){
                        //    _logFile << "bubulu (2): " << BiGraph::nodeIndex_to_nodeName(utg_3._nodes[0]) << endl;
                        //    getchar();
                        //}

                    }
                    */

                    /*
                    #pragma omp critical(superbubble)
                    {

                        for(u_int32_t nodeIndex : utg_2._nodes){
                            if(nodeIndex == 11055044){
                                _logFile << "HAAAAA" << endl;
                                getchar();
                            }
                        }
                        for(u_int32_t nodeIndex : utg_3._nodes){
                            if(nodeIndex == 11055044){
                                _logFile << "HAAAAA" << endl;
                                getchar();
                            }
                        }
                        //_logFile << "-----" << endl;
                        //_logFile << "equal: " << utg_2._index << " " << _unitigs[utg_2._index]._quality << " " << _unitigs[utg_2._index]._nbNodes << " " << _unitigs[utg_2._index]._abundance << " " << _unitigs[utg_2._index]._startNode << endl;
                        //_logFile << "equal: " << utg_3._index << " " << _unitigs[utg_3._index]._quality << " " << _unitigs[utg_3._index]._nbNodes << " " << _unitigs[utg_3._index]._abundance << " " << _unitigs[utg_3._index]._startNode << endl;
                        //_logFile << "keep: " << bubbleSides[0]._unitigIndex << " " << unitigNodes[0] << endl;
                    }
                    */

                    
                    vector<u_int32_t> unitigNodes; 

                    //remove bubble
                    if(utg_2._abundance > utg_3._abundance){
                        _graph->getUnitigNodes(utg_3, unitigNodes);

                        //_isVisited[utg_3._startNode] = true;
                        //_isVisited[utg_3._endNode] = true;
                        //_isVisited[nodeIndex_toReverseDirection(utg_3._startNode)] = true;
                        //_isVisited[nodeIndex_toReverseDirection(utg_3._endNode)] = true;
                    }
                    else if(utg_2._abundance < utg_3._abundance){
                        _graph->getUnitigNodes(utg_2, unitigNodes);


                        //_isVisited[utg_2._startNode] = true;
                        //_isVisited[utg_2._endNode] = true;
                        //_isVisited[nodeIndex_toReverseDirection(utg_2._startNode)] = true;
                        //_isVisited[nodeIndex_toReverseDirection(utg_2._endNode)] = true;

                    }
                    else{
                        
                        /*
                        u_int64_t sum2 = 0;
                        for(u_int32_t nodeIndex : _unitigs[utg_2._index]._nodes){
                            sum2 += nodeIndex;
                        }
                        u_int64_t sum3 = 0;
                        for(u_int32_t nodeIndex : _unitigs[utg_3._index]._nodes){
                            sum3 += nodeIndex;
                        }
                        */





                        vector<BubbleSide> bubbleSides;
                        bubbleSides.push_back({utg_2._index, _unitigs[utg_2._index]._nbNodes, _unitigs[utg_2._index]._startNode}); //_unitigs[utg_2._index]._quality
                        bubbleSides.push_back({utg_3._index, _unitigs[utg_3._index]._nbNodes, _unitigs[utg_3._index]._startNode}); //_unitigs[utg_3._index]._quality

                        std::sort(bubbleSides.begin(), bubbleSides.end(), BubbleSideComparator);

                        _graph->getUnitigNodes(_unitigs[bubbleSides[1]._unitigIndex], unitigNodes);
                        //_logFile << "keep: " << bubbleSides[0]._unitigIndex << endl;

                        //#pragma omp critical(superbubble)
                        //{
                            //_logFile << "-----" << endl;
                            //_logFile << "equal: " << utg_2._index << " " << _unitigs[utg_2._index]._quality << " " << _unitigs[utg_2._index]._nbNodes << " " << _unitigs[utg_2._index]._abundance << endl;
                            //_logFile << "equal: " << utg_3._index << " " << _unitigs[utg_3._index]._quality << " " << _unitigs[utg_3._index]._nbNodes << " " << _unitigs[utg_3._index]._abundance << endl;
                            //_logFile << "keep: " << bubbleSides[0]._unitigIndex << " " << unitigNodes[0] << endl;
                        //}

                    }
                    






                    for(u_int32_t node : unitigNodes){
                        sw._nodeIndexRemoved.push_back(node);
                        sw._nodeIndexRemoved.push_back(nodeIndex_toReverseDirection(node));
                        //_removedNodes.insert(node);
                        //_removedNodes.insert(nodeIndex_toReverseDirection(node));
                        //_saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(node));

                    }


                    isBubble = true;
                    _graph->simplifySuperbubble(sw, _removedNodes, _isVisited, _saveState);

                    if(isBubble) break;
                }

                if(isBubble) break;
            }
            
            if(!isBubble){
                _graph->simplifySuperbubble(sw, _removedNodes, _isVisited, _saveState);
                return;
            }
        }

    };

    /*
    u_int64_t removeSmallLoop(float maxLength, SaveState2& saveState){

        unordered_set<u_int32_t> removedUnitigs;

        //_logFile << "removing small loops" << endl;
        u_int64_t nbRemoved = 0;

        //vector<u_int32_t> unitigNodes;

        //vector<bool> isVisited(_graphSuccessors->_nbNodes, false);
        bool dummy = false;


		unordered_set<u_int32_t> writtenUnitigs;
        vector<UnitigTip> unitigTips;

        if(_unitigIndexToClean.size() == 0){
            for(Unitig& u : _unitigs){
                if(u._startNode == -1) continue;
                //if(u._startNode % 2 != 0) continue;

                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._startNode)) != writtenUnitigs.end()) continue;
                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._endNode)) != writtenUnitigs.end()) continue;

                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._startNode));
                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._endNode));
                

                unitigTips.push_back({u._index, u._startNode, u._length});
            }

        }
        else{

            for(u_int32_t unitigIndex : _unitigIndexToClean){
                const Unitig& u = _unitigs[unitigIndex];
                if(u._startNode == -1) continue;
                //if(u._length > maxLength) continue;
                unitigTips.push_back({u._index, u._startNode, u._length});
            }
        }

        //std::sort(unitigTips.begin(), unitigTips.end(), UnitigTipComparator_ByLength_Reverse);
        

        for(const UnitigTip& unitigTip : unitigTips){


            const Unitig& unitig_source = _unitigs[unitigTip._unitigIndex];

        //for(Unitig& unitig_source : _unitigs){
            //if(unitig_source._startNode == -1) continue;


            //if(isVisited[unitig_source._startNode]) continue;
            //if(isVisited[unitig_source._endNode]) continue;


            vector<u_int32_t> neighbors;
            getSuccessors(unitig_source._endNode, 0, neighbors);


            if(neighbors.size() < 2) continue;
            if(neighbors.size() > 4) continue;

            bool foundSmallLoop = false;

            for(size_t i=0; i<neighbors.size(); i++){

                Unitig& unitig_dest = nodeIndex_to_unitig(neighbors[i]);

                for(size_t j=0; j<neighbors.size(); j++){
                    if(i == j) continue;

                    Unitig& unitig_loop = nodeIndex_to_unitig(neighbors[j]);
                    if(unitig_loop._length > maxLength) continue;



                    vector<u_int32_t> predecessors;
                    getPredecessors(unitig_loop._startNode, 0, predecessors);
                    if(predecessors.size() != 2) continue;
                    vector<u_int32_t> successors;
                    getSuccessors(unitig_loop._endNode, 0, successors);
                    if(successors.size() != 2) continue;


                    if((nodeIndex_to_unitigIndex(predecessors[0]) == unitig_loop._index && nodeIndex_to_unitigIndex(predecessors[1]) == unitig_source._index) || (nodeIndex_to_unitigIndex(predecessors[0]) == unitig_source._index && nodeIndex_to_unitigIndex(predecessors[1]) == unitig_loop._index)){
                        if((nodeIndex_to_unitigIndex(successors[0]) == unitig_loop._index && nodeIndex_to_unitigIndex(successors[1]) == unitig_dest._index) || (nodeIndex_to_unitigIndex(successors[0]) == unitig_dest._index && nodeIndex_to_unitigIndex(successors[1]) == unitig_loop._index)){
                            
                            #ifdef PRINT_DEBUG_SIMPLIFICATION
                                _logFile << "Small loop: " <<  _graphSuccessors->nodeToString(unitig_loop._startNode) << " " << _graphSuccessors->nodeToString(unitig_loop._endNode)  << endl;
                            #endif
                            
                            removedUnitigs.insert(unitig_source._index);
                            removedUnitigs.insert(unitig_dest._index);
                            removedUnitigs.insert(unitig_loop._index);
                            removedUnitigs.insert(unitigIndex_toReverseDirection(unitig_source._index));
                            removedUnitigs.insert(unitigIndex_toReverseDirection(unitig_dest._index));
                            removedUnitigs.insert(unitigIndex_toReverseDirection(unitig_loop._index));

                            DbgEdge edge = {unitig_source._endNode, unitig_dest._startNode};
                            edge = edge.normalize();
                            _isEdgeRemoved.insert(edge);
                            saveState._isEdgeRemoved.push_back(edge);
                            edge = {unitig_loop._endNode, unitig_loop._startNode};
                            edge = edge.normalize();
                            _isEdgeRemoved.insert(edge);
                            saveState._isEdgeRemoved.push_back(edge);

                            
                            edge = {nodeIndex_toReverseDirection(unitig_dest._startNode), nodeIndex_toReverseDirection(unitig_source._endNode)};
                            edge = edge.normalize();
                            _isEdgeRemoved.insert(edge);
                            saveState._isEdgeRemoved.push_back(edge);
                            edge = {nodeIndex_toReverseDirection(unitig_loop._startNode), nodeIndex_toReverseDirection(unitig_loop._endNode)};
                            edge = edge.normalize();
                            _isEdgeRemoved.insert(edge);
                            saveState._isEdgeRemoved.push_back(edge);
                            

                            nbRemoved += 1;
                            foundSmallLoop = true;
                            break;
                            //isVisited[unitig_source._startNode] = true;
                            //isVisited[unitig_source._destNode] = true;
                        }
                    }

                }

                if(foundSmallLoop) break;
            }


            //#ifdef PRINT_DEBUG_SIMPLIFICATION
            //    _logFile << "\tSmall loop: " << _graphSuccessors->nodeIndex_to_nodeName(unitig._endNode, dummy) << endl;
            //#endif 

            //nbRemoved += 1;
        }

        removeUnitigs(removedUnitigs);

        return nbRemoved;
    }
    */


    u_int32_t nodeIndex_to_unitigIndex(u_int32_t nodeIndex){
        return _unitigs[_nodeToUnitig[nodeIndex]]._index;
    }

    Unitig& nodeIndex_to_unitig(u_int32_t nodeIndex){
        return _unitigs[_nodeToUnitig[nodeIndex]];
    }

    float getNodeUnitigAbundance(u_int32_t nodeIndex){
        return _unitigs[_nodeToUnitig[nodeIndex]]._abundance;
    }

    unordered_set<u_int32_t> _removedUnitigs;

    /*
    void indexUnitigData(vector<UnitigData>& unitigDatas){
        _unitigDatas2.resize(_unitigs.size());
        
        for(const Unitig& unitig : _unitigs){
            if(unitig._startNode == -1) continue;


        }

    }
    */

    void addUnitigGraphEdges(){

        _logFile << "Add unitig graph edges" << endl;
        _logFile << "Unitig graph nb nodes: " << _unitigGraph->_nodes.size() << endl;

        for(size_t i=0; i<_unitigGraph->_nodes.size(); i++){
            
            UnitigGraph::Node* node = _unitigGraph->_nodes[i];

            u_int32_t endNode = node->endNode();
            vector<u_int32_t> successors;
            getSuccessors(endNode, 0, successors);
                
            //_logFile << "c" << endl;

            for(u_int32_t nodeIndex : successors){
                u_int32_t toUnitigIndex = _unitigGraph->_nodeIndex_to_unitigIndex[nodeIndex];
                _unitigGraph->addSuccessor(node->_unitigIndex, toUnitigIndex);
            }

            u_int32_t startNode = node->startNode();
            vector<u_int32_t> predecessors;
            getPredecessors(startNode, 0, predecessors);
                
            //_logFile << "c" << endl;

            for(u_int32_t nodeIndex : predecessors){
                u_int32_t toUnitigIndex = _unitigGraph->_nodeIndex_to_unitigIndex[nodeIndex];
                _unitigGraph->addPredecessors(node->_unitigIndex, toUnitigIndex);
            }

        }

        for(size_t i=0; i<_unitigGraph->_nodes.size(); i++){
            
            UnitigGraph::Node* node = _unitigGraph->_nodes[i];

            std::sort(node->_successors.begin(), node->_successors.end(), UnitigGraph::NodeComparator);
            std::sort(node->_predecessors.begin(), node->_predecessors.end(), UnitigGraph::NodeComparator);

        }

    }

    unordered_set<u_int32_t> _unitigIndexToClean;
    double _abCircular;

    void compact(bool rebuild, const vector<UnitigData>& unitigDatas){


        _logFile << "\tCompacting" << endl;
        //if(rebuild && _rebuildInvalidUnitigs.size() == 0) return;

        if(!rebuild){
            if(_unitigGraph != nullptr){
                delete _unitigGraph;
            }

            _unitigGraph = new UnitigGraph(_nbNodes, _kminmerLengthMean, _kminmerOverlapMean, _kminmerLengthMean-_kminmerOverlapMean, _graphSuccessors->_nodeDatas);
        }

        _unitigIndexToClean.clear();

        #ifdef PRINT_DEBUG_SIMPLIFICATION
            _logFile << "\tCompacting" << endl;
        #endif

        vector<u_int32_t> nodes;

        if(rebuild){

            //_logFile << "lala1" << endl;
            //vector<u_int32_t> unitigIndexes;
            //for (u_int32_t unitigIndex : _removedUnitigs){
            //    unitigIndexes.push_back(unitigIndex);
            //}
            //std::sort(unitigIndexes.begin(), unitigIndexes.end());

            for(u_int32_t unitigIndex : _removedUnitigs){

                //_logFile << unitigIndex << " " << _unitigs.size() << endl;
                //_logFile << _unitigs[unitigIndex]._startNode << endl;
                //nodes.push_back(_unitigs[unitigIndex]._startNode);
                //_logFile << unitigIndex << endl;
                //vector<u_int32_t> unitigNodes; 
                //getUnitigNodes(_unitigs[unitigIndex], unitigNodes);
                for(u_int32_t nodeIndex : _unitigs[unitigIndex]._nodes){
                    if(_isNodeRemoved[nodeIndex]) continue;
                    //if(_isNodeValid2.find(nodeIndex) == _isNodeValid2.end()) continue;
                    //computeUnitig(nodeIndex, unitigDatas, rebuild);
                    nodes.push_back(nodeIndex);
                    break;
                }

                //_logFile << "done" << endl;

                //_unitigs.erase(unitigIndex);
            }

            //_logFile << "lala2" << endl;
            _removedUnitigs.clear();
            //for(u_int32_t invalidUnitigIndex : _rebuildInvalidUnitigs){
            //    Unitig& unitig = _unitigs[invalidUnitigIndex];
            //    unitig._startNode = -1;
            //}
        }
        else{



            //_nodeToUnitig = unordered_map<u_int32_t, u_int32_t>();
            _possibleTips.clear();
            _nodeToUnitig = vector<u_int32_t>(_graphSuccessors->_nbNodes, -1);
            //_nodeToUnitig.clear();
            _nextUnitigIndex = 0;
            _unitigs.clear();
            _unitigDatas2.clear();

            for(size_t nodeIndex=0; nodeIndex<_graphSuccessors->_nbNodes; nodeIndex++){
                if(!_isNodeRemoved[nodeIndex]){
                    nodes.push_back(nodeIndex);
                }
                //for (u_int32_t nodeIndex : _isNodeValid2){
            }
            //std::sort(nodes.begin(), nodes.end());


        }


        //_nodeToUnitig.resize(_graphSuccessors->_nbNodes, -1);
        //ofstream outfile(_outputDir + "/" + "tmp_compacted_graph.gfa");

        //vector<u_int32_t> nodeToUnitig(_graphSuccessors->_nbNodes, -1);
        //vector<Unitig> unitigs;
        //unordered_set<u_int32_t> nodes;
        //unordered_map<u_int32_t, u_int32_t> startNodes;
        //unordered_map<u_int32_t, u_int32_t> endNodes;

        //u_int64_t unitigIndex = 0;
        
        //vector<bool> isVisited(_graphSuccessors->_nbNodes, false);




        //size_t nodeIndex = _graphSuccessors->nodeName_to_nodeIndex(720, true);

        //for(size_t nodeIndex=0; nodeIndex<_graphSuccessors->_nbNodes; nodeIndex++){

        srand(time(NULL));
        //std::sort(nodes.begin(), nodes.end(), UnitigComparator_ByIndex);
        //std::random_shuffle(nodes.begin(), nodes.end());

        /*
        for (u_int32_t nodeIndex : nodes){
            computeUnitig(nodeIndex, unitigDatas, rebuild);
            //_logFile << "done" << endl;
        }
        */

        //_logFile << "lala3" << endl;

	    _abCircular = 0;
        
        UnitigFunctor functor(this, rebuild);
        size_t i=0;

        #pragma omp parallel num_threads(1)
        {

            UnitigFunctor functorSub(functor);

            while(true){

                u_int32_t nodeIndex = -1;
                bool isEof = false;

                #pragma omp critical
                {
                    
                    if(i >= nodes.size()){
                        isEof = true;
                    }
                    else{
                        nodeIndex = nodes[i];
                        //_logFile << nodeIndex << endl;
                    }

                    i += 1;
                }

                if(isEof) break;
                functorSub(nodeIndex);

            }
            

        }

        if(!rebuild){
            addUnitigGraphEdges();
        }
        //std::sort(_unitigs.begin(), _unitigs.end(), UnitigComparator_ByIndex);
        

        //_logFile << _nextUnitigIndex << endl;
        //_logFile << "\tCdone" << endl;


        u_int32_t unitigIndex = 0;
        #ifdef PRINT_DEBUG_SIMPLIFICATION
            _logFile << "Nb unitigs: " << _unitigs.size() << endl;
        #endif

        //if(_abCircular > 0){
        //    _logFile << _abCircular << endl;
        //    getchar();
        //}
        _logFile << "\tdone: " << _unitigs.size() << endl;
        
        
        /*
        for(u_int32_t unitigIndex : _possibleTips){
            _logFile << "-" << endl;
            for(u_int32_t nodeIndex : _unitigs[unitigIndex]._nodes){
                _logFile << nodeIndex << endl;
            }
        }
        _logFile << _possibleTips.size() << endl;
        _logFile << isTip(nodeIndex_to_unitigIndex(29512)) << endl;
        _logFile << isTip(nodeIndex_to_unitigIndex(22528)) << endl;
        getchar();
        */
        //_logFile << "Nb unitigs: " << _unitigs.size() << endl;
        //_logFile << "Unitigs: " << _unitigs.size() << " " << _nodeToUnitig.size() << endl;
        //getchar();
        /*
        for(Unitig& unitig : unitigs){

            int lala = _graphSuccessors->nodeIndex_to_nodeName(unitig._startNode, dummy);
            if(lala == 4452 && dummy){
                _logFile << "4452+ " << nodeToUnitig[unitig._startNode] << endl;
                getPredecessors(unitig._startNode, neighbors);
                for(u_int32_t nn : neighbors){
                    bool orientation;
                    int loulou = _graphSuccessors->nodeIndex_to_nodeName(nn, orientation);
                    _logFile << loulou << " " << orientation << "    " << nodeToUnitig[nn] << endl;
                }
            }
            else if(lala == 4453 && dummy){
                _logFile << "4453+ " << nodeToUnitig[unitig._startNode] << endl;
                getPredecessors(unitig._startNode, neighbors);
                for(u_int32_t nn : neighbors){
                    bool orientation;
                    int loulou = _graphSuccessors->nodeIndex_to_nodeName(nn, orientation);
                    _logFile << loulou << " " << orientation << "    " << nodeToUnitig[nn] << endl;
                }
            }
            
        }
        */

        /*
        ifstream infile(_inputGfaFilename);


        std::string line;
        vector<string>* fields = new vector<string>();
        vector<string>* fields_optional = new vector<string>();

        while (std::getline(infile, line)){
            
            GfaParser::tokenize(line, fields, '\t');
            
            //_logFile << (*fields)[0] << endl;

            //if((*fields)[0] == "S"){
            //    outfile << line << endl;
            //}
            //else 
            
            if((*fields)[0] == "L"){
                string& from = (*fields)[1];
                string& fromOrient = (*fields)[2];
                string& to = (*fields)[3];
                string& toOrient = (*fields)[4];

                u_int32_t fromName = stoull(from); //unitigName_to_id(from);
                u_int32_t toName = stoull(to);; //unitigName_to_id(to);

                u_int32_t fromNodeIndex = _graphSuccessors->nodeName_to_nodeIndex(fromName, fromOrient == "+");
                u_int32_t toNodeIndex = _graphSuccessors->nodeName_to_nodeIndex(toName, toOrient == "+");

               
                //if(nodes.find(fromNodeIndex) == nodes.end()) continue;
                //if(nodes.find(toNodeIndex) == nodes.end()) continue;

                //if(nodes[fromNodeIndex] == nodes[toNodeIndex]) continue;
                //if(fromNodeIndex == 5432 || toNodeIndex == 5432){
                  //  _logFile << nodes.find(fromNodeIndex) << " " << nodes.find(toNodeIndex) << endl;
                //}
                //if(nodes.find(fromNodeIndex) == nodes.end()) continue;
                //if(nodes.find(toNodeIndex) == nodes.end()) continue;
                //u_int32_t unitigIndex_from = nodeToUnitigs[fromNodeIndex];
                //u_int32_t unitigIndex_to = nodeToUnitigs[toNodeIndex];

                //if(unitigIndex_from == -1 || unitigIndex_to == -1) continue;
                //_logFile << fromName << " " << unitigIndex_from << " " << toName << " " << unitigIndex_to << endl;
            
                //outfile << "L" << "\t" << nodes[fromNodeIndex] << "\t" <<  fromOrient << "\t" << nodes[toNodeIndex]  << "\t" << toOrient << "\t" << "0M" << endl;
            }
            //else {
            //    outfile << line << endl;
            //}
            
        }


        delete fields;
        delete fields_optional;
        */


        //outfile.close();
    }

	class UnitigFunctor {

		public:

		GraphSimplify* _graph;
        vector<u_int32_t>& _nodeToUnitig;
        vector<Unitig>& _unitigs;
        bool _rebuild;

		UnitigFunctor(GraphSimplify* graph, bool rebuild) : _graph(graph), _nodeToUnitig(graph->_nodeToUnitig), _unitigs(graph->_unitigs){
            _rebuild = rebuild;
		}

		UnitigFunctor(const UnitigFunctor& copy) : _graph(copy._graph), _nodeToUnitig(copy._graph->_nodeToUnitig), _unitigs(copy._graph->_unitigs){
            _rebuild = copy._rebuild;
		}

		~UnitigFunctor(){
		}

		void operator () (u_int32_t nodeIndex) {


            bool exist = false;
            #pragma omp critical(unitig)
            {
                if(_nodeToUnitig[nodeIndex] != -1) exist = true;
                //if(_nodeToUnitig.find(nodeIndex) != _nodeToUnitig.end()) exist = true;
            }

            /*
            if(nodeIndex == 11838996){
                cout << "haha " <<  exist << " " << _nodeToUnitig[nodeIndex] << endl;

                for(u_int32_t nodeIndex : _graph->_unitigs[_nodeToUnitig[nodeIndex]]._nodes){
                    cout << nodeIndex << endl;
                }
            }
            */

            if(exist) return;

            vector<u_int32_t> neighbors;
            bool dummy = false;


            u_int32_t startNode = nodeIndex;
            u_int32_t endNode = nodeIndex;

            bool isCircular = false;

            int lala1 = 0;
            int lala2 = 0;



            u_int32_t smallestNodeIndex = -1;

            //forward
            while(true){
                //_logFile << endNode << " " << nodeIndex << endl;

                if(endNode < smallestNodeIndex){
                    smallestNodeIndex = endNode;
                }

                _graph->getSuccessors(endNode, 0, neighbors);


                //_logFile << "lala1: " << neighbors.size() << endl;
                if(neighbors.size() != 1) break;

                //_logFile << endNode << " " << neighbors[0] << endl;

                u_int32_t successor = neighbors[0];
                _graph->getPredecessors(successor, 0, neighbors);
                //_logFile << BiGraph::nodeIndex_to_nodeName(successor) << " " << neighbors.size() << endl;
                //_logFile << "lala2: " << neighbors.size() << endl;
                if(neighbors.size() != 1) break;

                if(successor == nodeIndex){
                    endNode = successor;
                    isCircular = true;
                    //_logFile << "circular" << endl;
                    break; //Circular
                }


                endNode = successor;
                lala1 += 1;

            }

            if(!isCircular){

                //backward
                while(true){
                    _graph->getPredecessors(startNode, 0, neighbors);
                    //_logFile << "loulou1: " << neighbors.size() << endl;
                    if(neighbors.size() != 1) break;


                    u_int32_t predecessor = neighbors[0];
                    _graph->getSuccessors(predecessor, 0, neighbors);
                    //_logFile << "loulou2: " << neighbors.size() << endl;
                    if(neighbors.size() != 1) break;

                    startNode = predecessor;

                    lala2 += 1;

                }

            }
            else{
                startNode = smallestNodeIndex;
                endNode = smallestNodeIndex;

            }


            u_int32_t abundance_sum = 0;
            u_int32_t abundance_max = 0;

            u_int32_t length = 0;
            u_int32_t node = startNode;
            //_logFile << "---------------" << endl;

            //_logFile << BiGraph::nodeIndex_to_nodeName(startNode) << " " << BiGraph::nodeIndex_to_nodeName(endNode) << endl;


            size_t i=0;
            u_int32_t nbNodes = 0;
            u_int32_t lastNode = -1;

            vector<u_int32_t> nodes;
            vector<u_int32_t> nodesRC;
            u_int32_t qualitySum = 0;
            u_int32_t minQuality = -1;
            u_int32_t maxQuality = 0;
            //if(unitigIndex == 114)
            //_logFile << "----" << endl;

            vector<float> abundances;
            while(true){

                //if(unitigIndex == 114){
                //    _logFile << "HIIII: " << BiGraph::nodeIndex_to_nodeName(node) << " " << length << endl;
                //}

                //_logFile << _graphSuccessors->nodeIndex_to_nodeName(node, dummy) << endl;
                u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(node);

                /*
                u_int32_t quality = _graph->_graphSuccessors->_nodeDatas[nodeName]._quality;
                if(quality < minQuality){
                    minQuality = quality;
                }
                if(quality > maxQuality){
                    maxQuality = quality;
                }

                qualitySum += quality;
                */

                if(_graph->_graphSuccessors->_nodeDatas[nodeName]._abundance > abundance_max){
                    abundance_max = _graph->_graphSuccessors->_nodeDatas[nodeName]._abundance;
                }

                if(i == 0){
                    length += _graph->_graphSuccessors->_nodeDatas[nodeName]._length;
                    //_logFile << "lala: " << length << endl;
                }
                else{
                    //u_int16_t overlapLength = _graph->_graphSuccessors->getOverlap(lastNode, node);
                    //if(_kminmerSize > 4) _logFile << _graphSuccessors->_nodeLengths[nodeName] << " " << overlapLength << endl;
                    length += _graph->_graphSuccessors->_nodeDatas[nodeName]._length - _graph->_kminmerOverlapMean;
                    //_logFile << "\t" << _graphSuccessors->_nodeLengths[nodeName] << " " << overlapLength << " " << (_graphSuccessors->_nodeLengths[nodeName] - overlapLength) << endl;
                }

                //if(BiGraph::nodeIndex_to_nodeName(startNode) == 5304){
                //    _logFile << "\t" << length << endl;
                //}
                abundance_sum += _graph->_graphSuccessors->_nodeDatas[nodeName]._abundance;
                //nbNodes += 1;
                abundances.push_back(_graph->_graphSuccessors->_nodeDatas[nodeName]._abundance);

                _graph->getSuccessors(node, 0, neighbors);

                //if(unitigIndex == 114)
                //_logFile << neighbors.size() << endl;

                //_logFile << "lili: " << node << " " << neighbors.size() << endl;

                //isVisited[node] = true;
                nodes.push_back(node);
                nodesRC.push_back(nodeIndex_toReverseDirection(node));
                nbNodes += 1;

                //if(isCircular){
                    //_logFile << "\tnode: " << BiGraph::nodeIndex_to_nodeName(node) << endl;
                //}

                if(isCircular){
                    if(i == 0){ 
                        if(neighbors[0] == endNode) break; //Unitig with single node circular, or single node without edges
                    }
                    else{
                        if(node == endNode) break;
                    }
                }
                else{
                    if(node == endNode) break;
                }

                lastNode = node;
                node = neighbors[0];

                i += 1;


            }

            if(isCircular){
                abundance_sum -= _graph->_graphSuccessors->_nodeDatas[BiGraph::nodeIndex_to_nodeName(nodes[nodes.size()-1])]._abundance;
                abundances.pop_back();
            }


            std::reverse(nodesRC.begin(), nodesRC.end());
            float median = _graph->compute_median_float(abundances);
            float mean = ((float)abundance_sum) / ((float)abundances.size());

            /*
            if(isCircular){
                //_logFile << "----" << endl;
                for(u_int32_t nodeIndex : nodes){
                    _graph->_abCircular += _graph->getNodeUnitigAbundance(nodeIndex);
                    //_logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << " ";
                }
                //_logFile << endl;
                //median = 0;
                //mean = 0;
            }
            */

            /*
            if(startNode == 162558){
                _logFile << "-------------" << endl;
                _logFile << startNode << " " << endNode << endl;
                _logFile << lala1 << " " << lala2 << endl;
                getSuccessors(endNode, 0, neighbors);
                _logFile << "nb successors: " << neighbors.size() << endl;
                getPredecessors(startNode, 0, neighbors);
                _logFile << "nb predecessors: " << neighbors.size() << endl;
                //exit(1);
            }
            */

            //float abundance = ((float) abundance_sum) / nbNodes;
            //_logFile << abundance << endl;
            //if(_graphSuccessors->nodeIndex_to_nodeName(nodeIndex, dummy) == 4453){
            //    _logFile << _graphSuccessors->nodeIndex_to_nodeName(startNode, dummy) << " " << dummy << " " << _graphSuccessors->nodeIndex_to_nodeName(endNode, dummy) << " " << dummy << endl;
            //}

            //if(nodeIndex % 2 == 0) continue;

            //_logFile << "Unitig: " << BiGraph::nodeIndex_to_nodeName(startNode) << " " << length << endl;
            //_logFile << BiGraph::nodeIndex_to_nodeName(startNode) << " " << BiGraph::nodeIndex_to_nodeName(endNode) << endl;

            #pragma omp critical(unitig)
            {


                //_logFile << "1" << endl;
                bool isValid = false; //_nodeToUnitig[nodes[0]] == -1 || _nodeToUnitig[nodes[nodes.size()-1]] == -1;// _nodeToUnitig.find(nodes[0]) == _nodeToUnitig.end();
                
                for(u_int32_t nodeIndex : nodes){
                    if(_nodeToUnitig[nodeIndex] == -1){
                        isValid = true;
                        break;
                    }
                }

                /*
                bool exists = false;
                for(u_int32_t nodeIndex : nodes){
                    if(nodeIndex == 11838996){
                        exist = true;
                    }
                }
                if(exist){
                    cout << "-----" << endl;
                    for(u_int32_t nodeIndex : nodes){
                        cout << nodeIndex << endl;
                    }
                    cout << isValid << endl;
                }
                exist = false;
                for(u_int32_t nodesRC : nodes){
                    if(nodeIndex == 11838996){
                        exist = true;
                    }
                }
                if(exist){
                    cout << "-----" << endl;
                    for(u_int32_t nodeIndex : nodesRC){
                        cout << nodeIndex << endl;
                    }
                    cout << isValid << endl;
                }
                */

                /*
                bool isHere = false;
                for(u_int32_t nodeIndex : nodes){
                    if(nodeIndex == 7294223){
                        _logFile << "lalalalala: " << isValid << endl;
                        isHere = true;
                    }
                }

                if(isHere){
                    for(u_int32_t nodeIndex : nodes){
                        _logFile << nodeIndex << " " << _nodeToUnitig[nodeIndex] << endl;
                    }
                    getchar();
                }
                */

                //_logFile << "2" << endl;

                if(isValid){
                    //_logFile << "---" << endl;

                    //_logFile << "3" << endl;

                    u_int32_t unitigIndexRC = _graph->_nextUnitigIndex + 1;

                    u_int64_t sum = 0;
                    for(u_int32_t nodeIndex : nodes){

                        if(_rebuild){
                            u_int32_t utg = _nodeToUnitig[nodeIndex];
                            //_logFile << utg << " " << _unitigs.size() << endl;
                            if(utg != -1 && utg != _graph->_nextUnitigIndex && utg != unitigIndexRC){
                                //_logFile << utg << endl;
                                for(u_int32_t nodeIndex : _unitigs[utg]._nodes){
                                    //if(nodeIndex == 7294223){
                                    //    _logFile << "lol" << endl;
                                    //    getchar();
                                    //}
                                    _nodeToUnitig[nodeIndex] = -1;
                                    _nodeToUnitig[nodeIndex_toReverseDirection(nodeIndex)] = -1;

                                }

                                _unitigs[utg]._startNode = -1;
                                _unitigs[utg]._endNode = -1;
                                _unitigs[_graph->unitigIndex_toReverseDirection(utg)]._startNode = -1;
                                _unitigs[_graph->unitigIndex_toReverseDirection(utg)]._endNode = -1;


                                //if(_graph->_unitigIndexToClean.find(utg) != _graph->_unitigIndexToClean.end()){
                                //    _graph->_unitigIndexToClean.erase(utg);
                                //}
                                //if(_graph->_possibleTips.find(utg) != _graph->_possibleTips.end()){
                                //    _graph->_possibleTips.erase(utg);
                                //}
                            }
                        }

                        _nodeToUnitig[nodeIndex] = _graph->_nextUnitigIndex;
                        _nodeToUnitig[nodeIndex_toReverseDirection(nodeIndex)] = unitigIndexRC;
                        sum += nodeIndex;


                        //_logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                    }


                    //_logFile << "4" << endl;
                    //_nodeToUnitig[startNode] = _graph->_nextUnitigIndex;
                    //_nodeToUnitig[endNode] = _graph->_nextUnitigIndex;
                    /*
                    for(size_t i=0; i<nodes.size(); i++){
                        
                        if(i == 0){
                            
                            vector<u_int32_t> preds;
                            _graph->getPredecessors(nodes[i], 0, preds);
                            if(preds.size() == 1){
                                _logFile << "NOOOO " <<  BiGraph::nodeIndex_to_nodeName(nodes[i]) << endl;
                                getchar();
                            }
                            
                        }
                        else if(i == nodes.size()-1){


                            vector<u_int32_t> preds;
                            _graph->getPredecessors(nodes[i], 0, preds);
                            if(preds.size() != 1){
                                _logFile << "NOOOO " <<  BiGraph::nodeIndex_to_nodeName(nodes[i]) << endl;
                                getchar();
                            }
                            

                        }
                        else{
                            vector<u_int32_t> succs;
                            _graph->getSuccessors(nodes[i], 0, succs);
                            if(succs.size() != 1){
                                _logFile << "NOOOO " <<  BiGraph::nodeIndex_to_nodeName(nodes[i]) << endl;
                                getchar();
                            }
                        }

                    }
                    */

                    //_nodeToUnitig[nodeIndex_toReverseDirection(startNode)] = unitigIndexRC;
                    //_nodeToUnitig[nodeIndex_toReverseDirection(endNode)] = unitigIndexRC;



                    if(isCircular && nodes.size() > 1){
                        nodes.pop_back();
                        nodesRC.erase(nodesRC.begin());
                    }
                    _graph->_unitigGraph->addNode(nodes, median, length);
                    _graph->_unitigGraph->addNode(nodesRC, median, length);

                    _unitigs.push_back({_graph->_nextUnitigIndex, startNode, endNode, median, length, nbNodes, nodes});//, maxQuality, nodeIndex, sum*nbNodes});
                    _unitigs.push_back({unitigIndexRC, nodeIndex_toReverseDirection(endNode), nodeIndex_toReverseDirection(startNode), median, length, nbNodes, nodesRC});//, maxQuality, nodeIndex, sum*nbNodes});

                    if(_rebuild){
                        _graph->_unitigIndexToClean.insert(_graph->_nextUnitigIndex);
                        _graph->_unitigIndexToClean.insert(unitigIndexRC);

                        if(_graph->isTip(_graph->_nextUnitigIndex)){
                            _graph->_possibleTips.insert(_graph->_nextUnitigIndex);
                            _graph->_possibleTips.insert(unitigIndexRC);
                        }
                    }
                    
                    _graph->_nextUnitigIndex += 2;
                }

//
            }

            
            /*
            //---------- Unitig supporting reads
            unordered_set<u_int64_t> readIndexes_unique;
            for(u_int32_t nodeIndex : nodes){

                const UnitigData& unitigData = unitigDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex)];
                //_logFile << unitigData._readIndexes.size() << endl;
                for(u_int64_t readIndex : unitigData._readIndexes){
                    readIndexes_unique.insert(readIndex);
                }
            }

            vector<u_int64_t> readIndexes;
            for(u_int32_t readIndex : readIndexes_unique){
                readIndexes.push_back(readIndex);
            }
            std::sort(readIndexes.begin(), readIndexes.end());

            _unitigDatas2.push_back({0, readIndexes});
            _unitigDatas2.push_back({0, readIndexes}); //!!!!memory a gagner ici: necessaire car on distingue un unitig et son reverse
            //---------- Unitig supporting reads
            */

            
            //outfile << "S" << "\t" << unitigIndex << "\t" << "*" << endl;





            //_logFile << _graphSuccessors->nodeIndex_to_nodeName(startNode, dummy) << endl;
            //_logFile << _graphSuccessors->nodeIndex_to_nodeName(endNode, dummy) << endl;
            //_logFile << _graphSuccessors->nodeIndex_to_nodeName(neighbors[0], dummy) << endl;
            //getPredecessors(nodeName, neighbors);
            //_logFile << _graphSuccessors->nodeIndex_to_nodeName(neighbors[0], dummy) << endl;

        }

	};





    /*
    void computeUnitig(u_int32_t nodeIndex, const vector<UnitigData>& unitigDatas, bool rebuild){

        //if(_nodeToUnitig.find(nodeIndex) != _nodeToUnitig.end()) return;
        
        vector<u_int32_t> neighbors;
        bool dummy = false;

        //_logFile << nodeIndex << " " << _isNodeValid2.size() << endl;
        //u_int32_t nodeIndex = *it;
        //_logFile << nodeIndex << " " << _isNodeValid2.size() << endl;
        //if(nodeIndex % 10000 == 0){
        //    _logFile << nodeIndex << " " << _graphSuccessors->_nbNodes << endl;
        //}
        //if(nodeIndex % 2 == 1) continue;

        //if(!_isNodeValid[nodeIndex]) continue;
        //if(isVisited[nodeIndex]) continue;

        //isVisited[nodeIndex] = true;
        //u_int32_t nodeName = _graphSuccessors->nodeName_to_nodeIndex(7701, true);

        //_logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
        u_int32_t startNode = nodeIndex;
        u_int32_t endNode = nodeIndex;

        bool isCircular = false;

        int lala1 = 0;
        int lala2 = 0;


        //bool isRepeat = false;

        //if(_repeatedNodenames.size() > 0 && _repeatedNodenames.find(BiGraph::nodeIndex_to_nodeName(nodeIndex)) != _repeatedNodenames.end()){
        //    isRepeat = true;
        //}

        //forward
        while(true){
            //_logFile << endNode << " " << nodeIndex << endl;

            getSuccessors(endNode, 0, neighbors);



            //_logFile << "lala1: " << neighbors.size() << endl;
            if(neighbors.size() != 1) break;
            if(neighbors[0] == nodeIndex){
                endNode = neighbors[0];
                isCircular = true;
                //_logFile << "circular" << endl;
                break; //Circular
            }
            //_logFile << endNode << " " << neighbors[0] << endl;

            u_int32_t successor = neighbors[0];
            getPredecessors(successor, 0, neighbors);
            //_logFile << BiGraph::nodeIndex_to_nodeName(successor) << " " << neighbors.size() << endl;
            //_logFile << "lala2: " << neighbors.size() << endl;
            if(neighbors.size() != 1) break;



            //_logFile << successor << " " << neighbors[0] << endl;

            //_logFile << "1: " << BiGraph::nodeIndex_to_nodeName(successor) << endl;
            endNode = successor;
            lala1 += 1;

            //_logFile << "1" << endl;
        }

        if(!isCircular){

            //backward
            while(true){
                getPredecessors(startNode, 0, neighbors);
                //_logFile << "loulou1: " << neighbors.size() << endl;
                if(neighbors.size() != 1) break;



                //if(neighbors[0] == nodeIndex){
                //    startNode = neighbors[0];
                //    break; //Circular, todo: don't need to backward if circular
                //}

                //_logFile << startNode << " " << neighbors[0] << endl;

                u_int32_t predecessor = neighbors[0];
                getSuccessors(predecessor, 0, neighbors);
                //_logFile << "loulou2: " << neighbors.size() << endl;
                if(neighbors.size() != 1) break;


                //_logFile << predecessor << " " << neighbors[0] << endl;

                startNode = predecessor;

                lala2 += 1;

            }

        }

        
        //if(isCircular){
        //_logFile << startNode << " " << endNode << endl;
        //}

        //u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(startNode, dummy);

        //startNodes[startNode] = unitigIndex;
        //endNodes[endNode] = unitigIndex;
        //nodes.insert(endNode);



        //isVisited[startNode] = true;
        //isVisited[endNode] = true;

        //_logFile << unitigIndex << " " << _graphSuccessors->nodeIndex_to_nodeName(startNode, dummy) << " " << _graphSuccessors->nodeIndex_to_nodeName(endNode, dummy) << endl;
        //int lala = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex, dummy);
        //if(lala == 4452 && dummy){
        //    _logFile << "lal " << _graphSuccessors->nodeIndex_to_nodeName(startNode, dummy) << " " << dummy << " " << _graphSuccessors->nodeIndex_to_nodeName(endNode, dummy) << " " << dummy << endl;
        //}
        //lala = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex, dummy);
        //if(lala == 4452 && !dummy){
        //    _logFile << "loul " << _graphSuccessors->nodeIndex_to_nodeName(startNode, dummy) << " " << dummy << " " << _graphSuccessors->nodeIndex_to_nodeName(endNode, dummy) << " " << dummy << endl;
        //}


        u_int32_t abundance_sum = 0;
        u_int32_t abundance_max = 0;

        u_int32_t length = 0;
        u_int32_t node = startNode;
        //_logFile << "---------------" << endl;

        //_logFile << BiGraph::nodeIndex_to_nodeName(startNode) << " " << BiGraph::nodeIndex_to_nodeName(endNode) << endl;
        
        size_t i=0;
        u_int32_t nbNodes = 0;
        u_int32_t lastNode = -1;

        vector<u_int32_t> nodes;
        vector<u_int32_t> nodesRC;
        //if(unitigIndex == 114)
        //_logFile << "----" << endl;

        vector<float> abundances;
        while(true){

            //if(unitigIndex == 114){
            //    _logFile << "HIIII: " << BiGraph::nodeIndex_to_nodeName(node) << " " << length << endl;
            //}

            //_logFile << _graphSuccessors->nodeIndex_to_nodeName(node, dummy) << endl;
            u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(node);

            if(_graphSuccessors->_nodeAbundances[nodeName] > abundance_max){
                abundance_max = _graphSuccessors->_nodeAbundances[nodeName];
            }

            if(i == 0){
                length += _graphSuccessors->_nodeLengths[nodeName];
                //_logFile << "lala: " << length << endl;
            }
            else{
                //u_int16_t overlapLength = _graphSuccessors->getOverlap(lastNode, node);
                //if(_kminmerSize > 4) _logFile << _graphSuccessors->_nodeLengths[nodeName] << " " << overlapLength << endl;
                length += _graphSuccessors->_nodeLengths[nodeName] - _kminmerOverlapMean;
                //_logFile << "\t" << _graphSuccessors->_nodeLengths[nodeName] << " " << overlapLength << " " << (_graphSuccessors->_nodeLengths[nodeName] - overlapLength) << endl;
            }

            //if(BiGraph::nodeIndex_to_nodeName(startNode) == 5304){
            //    _logFile << "\t" << length << endl;
            //}
            abundance_sum += _graphSuccessors->_nodeAbundances[nodeName];
            //nbNodes += 1;
            abundances.push_back(_graphSuccessors->_nodeAbundances[nodeName]);

            getSuccessors(node, 0, neighbors);

            //if(unitigIndex == 114)
            //_logFile << neighbors.size() << endl;

            //_logFile << "lili: " << node << " " << neighbors.size() << endl;

            //isVisited[node] = true;
            nodes.push_back(node);
            nodesRC.push_back(nodeIndex_toReverseDirection(node));
            nbNodes += 1;

            //if(isCircular){
                //_logFile << "\tnode: " << BiGraph::nodeIndex_to_nodeName(node) << endl;
            //}

            if(isCircular){
                if(i == 0){ 
                    if(neighbors[0] == endNode) break; //Unitig with single node circular, or single node without edges
                }
                else{
                    if(node == endNode) break;
                }
            }
            else{
                if(node == endNode) break;
            }

            lastNode = node;
            node = neighbors[0];

            i += 1;


        }


        std::reverse(nodesRC.begin(), nodesRC.end());
        float median = Utils::compute_median_float(abundances);
        //((float)abundance_sum) / ((float)nbNodes)


        //float abundance = ((float) abundance_sum) / nbNodes;
        //_logFile << abundance << endl;
        //if(_graphSuccessors->nodeIndex_to_nodeName(nodeIndex, dummy) == 4453){
        //    _logFile << _graphSuccessors->nodeIndex_to_nodeName(startNode, dummy) << " " << dummy << " " << _graphSuccessors->nodeIndex_to_nodeName(endNode, dummy) << " " << dummy << endl;
        //}

        //if(nodeIndex % 2 == 0) continue;

        //_logFile << "Unitig: " << BiGraph::nodeIndex_to_nodeName(startNode) << " " << length << endl;
        //_logFile << BiGraph::nodeIndex_to_nodeName(startNode) << " " << BiGraph::nodeIndex_to_nodeName(endNode) << endl;

        //#pragma omp critical
        //{
            bool isValid = true; //_nodeToUnitig.find(nodes[0]) == _nodeToUnitig.end();
            
            if(isValid){
                _nodeToUnitig[startNode] = _nextUnitigIndex;
                _nodeToUnitig[endNode] = _nextUnitigIndex;
                u_int32_t unitigIndexRC = _nextUnitigIndex + 1;

                for(u_int32_t nodeIndex : nodes){
                    _nodeToUnitig[nodeIndex] = _nextUnitigIndex;
                    _nodeToUnitig[nodeIndex_toReverseDirection(nodeIndex)] = unitigIndexRC;
                }
                
                _nodeToUnitig[nodeIndex_toReverseDirection(startNode)] = unitigIndexRC;
                _nodeToUnitig[nodeIndex_toReverseDirection(endNode)] = unitigIndexRC;

                
                //_unitigs.push_back({_nextUnitigIndex, startNode, endNode, median, length, nbNodes, nodes});
                //_unitigs.push_back({unitigIndexRC, nodeIndex_toReverseDirection(endNode), nodeIndex_toReverseDirection(startNode), median, length, nbNodes, nodesRC});

                if(rebuild){
                    _unitigIndexToClean.insert(_nextUnitigIndex);
                    _unitigIndexToClean.insert(unitigIndexRC);
                }
                
            }


        //}


        
        //outfile << "S" << "\t" << unitigIndex << "\t" << "*" << endl;





        _nextUnitigIndex += 2;
        //_logFile << _graphSuccessors->nodeIndex_to_nodeName(startNode, dummy) << endl;
        //_logFile << _graphSuccessors->nodeIndex_to_nodeName(endNode, dummy) << endl;
        //_logFile << _graphSuccessors->nodeIndex_to_nodeName(neighbors[0], dummy) << endl;
        //getPredecessors(nodeName, neighbors);
        //_logFile << _graphSuccessors->nodeIndex_to_nodeName(neighbors[0], dummy) << endl;
    }
    */

    void getSuccessors(u_int32_t n, float abundanceCutoff_min, vector<u_int32_t>& successors){

        //vector<u_int32_t> possibleSuccessors;


        bool dummy;
        successors.clear();

        const vector<AdjNode>& nodes = _graphSuccessors->_nodes[n];
        int lala = 0;
        //adjNode* node = _graphSuccessors->_nodes[n];

        //while (node != nullptr) {
        for(const AdjNode& node : nodes){

			u_int32_t nn = node._index;
            //if(!_isNodeValid[nn]){
            //if(_isNodeValid2.find(nn) == _isNodeValid2.end()){
            if(_isNodeRemoved[nn]) continue;

            //if(_isNodeInvalid_tmp.find(nn) != _isNodeInvalid_tmp.end()){
            //    continue;
            //}

            //if(_isNodeInvalid_tmp.find(nn) != _isNodeInvalid_tmp.end()) continue;
            //if(_allowedNodeIndex.size() > 0 && _allowedNodeIndex.find(BiGraph::nodeIndex_to_nodeName(nn)) == _allowedNodeIndex.end()) continue;

            if(node._isRemoved) continue;

            /*
            if((n == 329 && nn == 349) || (n == 349 && nn == 329)){
                _logFile << "lala: " << lala << " " << n << " " << nn << " " << node._isRemoved << " " << _graphSuccessors->_nodes[329][1]._index << " " << _graphSuccessors->_nodes[329][1]._isRemoved << endl;
                _logFile << nodes.size() << " " << _graphSuccessors->_nodes[329][0]._index << " " << _graphSuccessors->_nodes[329][1]._index << " " << _graphSuccessors->_nodes[329][2]._index << " " << _graphSuccessors->_nodes[329][3]._index << endl;
                getchar();
            }
            */
            
            //if(isEdgeRemoved(n, nn)) continue;
                //node = node->next;
            //    continue;
            //}
            

			//if(abundanceCutoff_min != 0 && getNodeUnitigAbundance(nn) < abundanceCutoff_min) continue;

            
            /*
            if(BiGraph::nodeName_to_nodeIndex(2068, false) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(2068, true) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(5645, false) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(5645, true) == nn){
                continue;
            }
            */
            //u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nn, dummy);
                
                
                //Abundance min cutoff
			//	node = node->next;
			//	continue;
			//}

            successors.push_back(nn);
            lala += 1;
            //node = node->next;
        }

        //if(abundanceCutoff_min != 0) applyAbundanceCutoff(successors, abundanceCutoff_min, successors);
    }
    /*
    void applyAbundanceCutoff(vector<u_int32_t> allSuccessors, float abundanceCutoff_min, vector<u_int32_t>& successors){

        successors.clear();
        if(allSuccessors.size() == 0) return;

        u_int32_t maxAbundance = 0;
        u_int32_t maxAbundanceNodeIndex = -1;

        for(u_int32_t nodeIndex : allSuccessors){
            if(getNodeUnitigAbundance(nodeIndex) > maxAbundance){
                maxAbundance = getNodeUnitigAbundance(nodeIndex);
                maxAbundanceNodeIndex = nodeIndex;
            }
        }

        successors.push_back(maxAbundanceNodeIndex);
        for(u_int32_t nodeIndex : allSuccessors){
            if(nodeIndex == maxAbundanceNodeIndex) continue;
            if(getNodeUnitigAbundance(nodeIndex) < abundanceCutoff_min) continue;
            successors.push_back(nodeIndex);
        }
    }

    void applyAbundanceCutoff_overlap(vector<AdjNode> allSuccessors, float abundanceCutoff_min, vector<AdjNode>& successors){

        successors.clear();
        if(allSuccessors.size() == 0) return;

        u_int32_t maxAbundance = 0;
        //u_int32_t maxAbundanceNodeIndex = -1;
        AdjNode maxAbundanceNodeIndex;

        for(const AdjNode& nodeIndex : allSuccessors){
            if(getNodeUnitigAbundance(nodeIndex._index) > maxAbundance){
                maxAbundance = getNodeUnitigAbundance(nodeIndex._index);
                maxAbundanceNodeIndex = nodeIndex;
            }
        }

        successors.push_back(maxAbundanceNodeIndex);
        for(const AdjNode& nodeIndex : allSuccessors){
            if(nodeIndex._index == maxAbundanceNodeIndex._index) continue;
            if(getNodeUnitigAbundance(nodeIndex._index) < abundanceCutoff_min) continue;
            successors.push_back(nodeIndex);
        }
    }*/

    void getSuccessors_overlap(u_int32_t n, float abundanceCutoff_min, vector<AdjNode>& successors){

        //_logFile << "succ overlap " << endl;

        bool dummy;
        successors.clear();

        vector<AdjNode>& nodes = _graphSuccessors->_nodes[n];
        //adjNode* node = _graphSuccessors->_nodes[n];

        //while (node != nullptr) {
        for(AdjNode& node : nodes){

			u_int32_t nn = node._index;
            //if(!_isNodeValid[nn]){
            if(_isNodeRemoved[nn]) continue;
            //if(_isNodeValid2.find(nn) == _isNodeValid2.end()){

            //if(_isNodeInvalid_tmp.find(nn) != _isNodeInvalid_tmp.end()){
            //    continue;
            //}


            //if(_isNodeInvalid_tmp.find(nn) != _isNodeInvalid_tmp.end()) continue;
            //if(_allowedNodeIndex.size() > 0 && _allowedNodeIndex.find(BiGraph::nodeIndex_to_nodeName(nn)) == _allowedNodeIndex.end()) continue;

            if(node._isRemoved) continue;
            //if(isEdgeRemoved(n, nn)) continue;
            //if(isEdgeRemoved(n, nn)){
                //node = node->next;
            //    continue;
            //}
            

			//if(abundanceCutoff_min != 0 && getNodeUnitigAbundance(nn) < abundanceCutoff_min) continue;
            
            /*
            if(BiGraph::nodeName_to_nodeIndex(2068, false) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(2068, true) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(5645, false) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(5645, true) == nn){
                continue;
            }
            */
            //u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nn, dummy);
			//if(abundanceCutoff_min != 0 && _unitigs[_nodeToUnitig[nn]]._abundance < abundanceCutoff_min){ //Abundance min cutoff
			//	node = node->next;
			//	continue;
			//}

            successors.push_back(node);
            //node = node->next;
        }

        //if(abundanceCutoff_min != 0) applyAbundanceCutoff_overlap(successors, abundanceCutoff_min, successors);
    }

    void getPredecessors_overlap(u_int32_t n, float abundanceCutoff_min, vector<AdjNode>& successors){

        //_logFile << "pred overlap " << endl;
        bool dummy;
        successors.clear();

        vector<AdjNode>& nodes = _graphSuccessors->_nodes[nodeIndex_toReverseDirection(n)];
        //adjNode* node = _graphSuccessors->_nodes[n];

        //while (node != nullptr) {
        for(AdjNode& node : nodes){

			u_int32_t nn = nodeIndex_toReverseDirection(node._index);
            AdjNode node_nn = {nn, _kminmerOverlapMean};

            //_logFile << "YEY " << BiGraph::nodeIndex_to_nodeName(nn) << endl;
            //if(!_isNodeValid[nn]){
            //if(_isNodeValid2.find(nn) == _isNodeValid2.end()){
            if(_isNodeRemoved[nn]) continue;

            //if(_isNodeInvalid_tmp.find(nn) != _isNodeInvalid_tmp.end()) continue;
            //if(_allowedNodeIndex.size() > 0 && _allowedNodeIndex.find(BiGraph::nodeIndex_to_nodeName(nn)) == _allowedNodeIndex.end()) continue;

            //if(_isNodeInvalid_tmp.find(nn) != _isNodeInvalid_tmp.end()){
            //    continue;
            //}


            if(node._isRemoved) continue;
            //if(isEdgeRemoved(n, nn)) continue;
            //if(isEdgeRemoved(n, nn)){
                //node = node->next;
            //    continue;
            //}
            

			//if(abundanceCutoff_min != 0 && getNodeUnitigAbundance(nn) < abundanceCutoff_min) continue;
            
            /*
            if(BiGraph::nodeName_to_nodeIndex(2068, false) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(2068, true) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(5645, false) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(5645, true) == nn){
                continue;
            }
            */
            //u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nn, dummy);
			//if(abundanceCutoff_min != 0 && _unitigs[_nodeToUnitig[nn]]._abundance < abundanceCutoff_min){ //Abundance min cutoff
			//	node = node->next;
			//	continue;
			//}

            successors.push_back(node_nn);
            //node = node->next;
        }

        //if(abundanceCutoff_min != 0) applyAbundanceCutoff_overlap(successors, abundanceCutoff_min, successors);
    }

    void getPredecessors(u_int32_t n, float abundanceCutoff_min, vector<u_int32_t>& predecessors){

        bool dummy;
        predecessors.clear();
        



        vector<AdjNode>& nodes = _graphSuccessors->_nodes[nodeIndex_toReverseDirection(n)];
        //adjNode* node = _graphSuccessors->_nodes[nodeIndex_toReverseDirection(n)];

        for(AdjNode& node : nodes){
        //while (node != nullptr) {

			u_int32_t nn = nodeIndex_toReverseDirection(node._index);
            //_logFile << "Pred: " << BiGraph::nodeIndex_to_nodeName(nn) << endl;

            //if(!_isNodeValid[nn]){
            //if(_isNodeValid2.find(nn) == _isNodeValid2.end()){
            if(_isNodeRemoved[nn]) continue;

            //if(_isNodeInvalid_tmp.find(nn) != _isNodeInvalid_tmp.end()) continue;

            //if(_allowedNodeIndex.size() > 0 && _allowedNodeIndex.find(BiGraph::nodeIndex_to_nodeName(nn)) == _allowedNodeIndex.end()) continue;
            //if(_isNodeInvalid_tmp.find(nn) != _isNodeInvalid_tmp.end()){
            //    continue;
            //}

            if(node._isRemoved) continue;
            //if(isEdgeRemoved(n, nn)) continue;
            //if(isEdgeRemoved(n, nn)){
                //node = node->next;
            //    continue;
            //}


			//if(abundanceCutoff_min != 0 && getNodeUnitigAbundance(nn) < abundanceCutoff_min) continue;

            /*
            if(BiGraph::nodeName_to_nodeIndex(2068, false) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(2068, true) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(5645, false) == nn){
                continue;
            }
            if(BiGraph::nodeName_to_nodeIndex(5645, true) == nn){
                continue;
            }*/

            //u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nn, dummy);
			//if(abundanceCutoff_min != 0 && _unitigs[_nodeToUnitig[nn]]._abundance < abundanceCutoff_min){ //Abundance min cutoff
			//	node = node->next;
			//	continue;
			//}

            predecessors.push_back(nn);
            //node = node->next;
        }

        //if(abundanceCutoff_min != 0) applyAbundanceCutoff(predecessors, abundanceCutoff_min, predecessors);
    }

    
    static u_int32_t nodeIndex_toReverseDirection(u_int32_t nodeIndex){
        bool orient;
        u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(nodeIndex, orient);

        u_int32_t nodeIndex_prev;
        if(orient){
            nodeIndex_prev = BiGraph::nodeName_to_nodeIndex(nodeName, false);
        }
        else{
            nodeIndex_prev = BiGraph::nodeName_to_nodeIndex(nodeName, true);
        }

        return nodeIndex_prev;
    }

    u_int32_t unitigIndex_toReverseDirection(u_int32_t unitigIndex){
        if(unitigIndex % 2 == 0){
            return unitigIndex+1;
        }
        else{
            return unitigIndex-1;
        }
        //return nodeIndex_to_unitigIndex(nodeIndex_toReverseDirection(_unitigs[unitigIndex]._startNode));

    }

    void getNeighbors(u_int32_t n, float abundanceCutoff_min, vector<u_int32_t>& neighbors){

        neighbors.clear();

        vector<u_int32_t> successors;
        getSuccessors(n, abundanceCutoff_min, successors);

        vector<u_int32_t> predecessors;
        getPredecessors(n, abundanceCutoff_min, predecessors);

        //_logFile << n << " " << successors.size() << " " << predecessors.size() << endl;
        neighbors.insert(neighbors.end(), successors.begin(), successors.end());
        neighbors.insert(neighbors.end(), predecessors.begin(), predecessors.end());
    }
    
    void getSuccessors_unitig(u_int32_t unitigIndex, float abundanceCutoff, vector<u_int32_t>& successors){

        //_logFile << unitigIndex << " " << _unitigs.size() << endl;
        successors.clear();

        //_logFile << "a" << endl;
        Unitig& unitig = _unitigs[unitigIndex];

        //_logFile << (unitig._endNode == -1)<< endl;
        //_logFile << BiGraph::nodeIndex_to_nodeName(unitig._endNode) << endl;
        vector<u_int32_t> successors_nodeIndex;
        getSuccessors(unitig._endNode, 0, successors_nodeIndex);
            
        //_logFile << "c" << endl;

        for(u_int32_t nodeIndex : successors_nodeIndex){

            //_logFile << nodeIndex << " " << _nodeToUnitig[nodeIndex] << endl;
            //if(_nodeToUnitig.find(nodeIndex) == _nodeToUnitig.end()) continue; //reinserting bubble
            //if(_isNodeInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isNodeInvalid_tmp.end()) continue;
            //if(_isUnitigInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isUnitigInvalid_tmp.end()) continue;

            //if(nodeIndex_to_unitigIndex(nodeIndex) == nodeIndex_to_unitigIndex(BiGraph::nodeName_to_nodeIndex(1302, true))) _logFile << "hhhhhhha" << endl;
            //if(nodeIndex_to_unitigIndex(nodeIndex) == nodeIndex_to_unitigIndex(BiGraph::nodeName_to_nodeIndex(1302, false))) _logFile << "hhhhhhha" << endl;
            //_logFile << (_isNodeInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isNodeInvalid_tmp.end()) << endl;

            //if(_isNodeInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isNodeInvalid_tmp.end()) continue;

            successors.push_back(nodeIndex_to_unitigIndex(nodeIndex));
        }

        //_logFile << "d" << endl;
    }

    void getPredecessors_unitig(u_int32_t unitigIndex, float abundanceCutoff, vector<u_int32_t>& predecessors){

        predecessors.clear();

        Unitig& unitig = _unitigs[unitigIndex];

        vector<u_int32_t> predecessors_nodeIndex;
        getPredecessors(unitig._startNode, 0, predecessors_nodeIndex);
            
        for(u_int32_t nodeIndex : predecessors_nodeIndex){

            //_logFile << nodeIndex << " " << _nodeToUnitig[nodeIndex] << endl;
            //if(_nodeToUnitig.find(nodeIndex) == _nodeToUnitig.end()) continue; //reinserting bubble
            //if(_isNodeInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isNodeInvalid_tmp.end()) continue;
            //if(_isUnitigInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isUnitigInvalid_tmp.end()) continue;

            predecessors.push_back(nodeIndex_to_unitigIndex(nodeIndex));
        }
    }

    void getSuccessors_unitig_overlap(u_int32_t unitigIndex, vector<AdjNode>& successors){

        //_logFile << unitigIndex << " " << _unitigs.size() << endl;
        successors.clear();

        //_logFile << "a" << endl;
        Unitig& unitig = _unitigs[unitigIndex];

        //_logFile << (unitig._endNode == -1)<< endl;
        //_logFile << BiGraph::nodeIndex_to_nodeName(unitig._endNode) << endl;
        vector<AdjNode> successors_nodeIndex;
        getSuccessors_overlap(unitig._endNode, 0, successors_nodeIndex);
            
        //_logFile << "c" << endl;

        for(const AdjNode& node : successors_nodeIndex){

            u_int32_t nodeIndex = node._index;
            //if(_nodeToUnitig.find(nodeIndex) == _nodeToUnitig.end()) continue; //reinserting bubble
            //if(_isNodeInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isNodeInvalid_tmp.end()) continue;
            //if(_isUnitigInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isUnitigInvalid_tmp.end()) continue;

            //if(nodeIndex_to_unitigIndex(nodeIndex) == nodeIndex_to_unitigIndex(BiGraph::nodeName_to_nodeIndex(1302, true))) _logFile << "hhhhhhha" << endl;
            //if(nodeIndex_to_unitigIndex(nodeIndex) == nodeIndex_to_unitigIndex(BiGraph::nodeName_to_nodeIndex(1302, false))) _logFile << "hhhhhhha" << endl;
            //_logFile << (_isNodeInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isNodeInvalid_tmp.end()) << endl;

            //if(_isNodeInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isNodeInvalid_tmp.end()) continue;

            successors.push_back({nodeIndex_to_unitigIndex(nodeIndex), _kminmerOverlapMean});
        }

        //_logFile << "d" << endl;
    }

    void getPredecessors_unitig_overlap(u_int32_t unitigIndex, vector<AdjNode>& predecessors){

        predecessors.clear();

        Unitig& unitig = _unitigs[unitigIndex];

        vector<AdjNode> predecessors_nodeIndex;
        getPredecessors_overlap(unitig._startNode, 0, predecessors_nodeIndex);
            
        for(const AdjNode& node : predecessors_nodeIndex){

            u_int32_t nodeIndex = node._index;

            //if(_nodeToUnitig.find(nodeIndex) == _nodeToUnitig.end()) continue; //reinserting bubble
            //if(_isNodeInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isNodeInvalid_tmp.end()) continue;
            //if(_isUnitigInvalid_tmp.find(nodeIndex_to_unitigIndex(nodeIndex)) != _isUnitigInvalid_tmp.end()) continue;

            predecessors.push_back({nodeIndex_to_unitigIndex(nodeIndex), _kminmerOverlapMean});
        }
    }
    /*
    u_int32_t in_degree(u_int32_t v){
        vector<u_int32_t> predecessors;
        getPredecessors(v, 0, predecessors);
        return predecessors.size();
    }
    
    u_int32_t out_degree(u_int32_t v){
        vector<u_int32_t> successors;
        getSuccessors(v, 0, successors);
        return successors.size();
    }
    */

    /*
    u_int32_t in_degree(u_int32_t unitigIndex){
        vector<u_int32_t> predecessors;
        getPredecessors_unitig(unitigIndex, predecessors);
        return predecessors.size();
    }
    
    u_int32_t out_degree(u_int32_t unitigIndex){
        vector<u_int32_t> successors;
        getSuccessors_unitig(unitigIndex, successors);
        return successors.size();
    }*/
    /*
    bool isEdgeRemoved(u_int32_t nodeIndex_from, u_int32_t nodeIndex_to){

        //DbgEdge edgeName = {BiGraph::nodeIndex_to_nodeName(nodeIndex_from), BiGraph::nodeIndex_to_nodeName(nodeIndex_to)};
        //if(_isEdgeUnsupported.find(edgeName.normalize()) != _isEdgeUnsupported.end()) return true;
        

        DbgEdge edge = {nodeIndex_from, nodeIndex_to};
		edge = edge.normalize();
        return _isEdgeRemoved.find(edge) != _isEdgeRemoved.end();
    }
    */


    void getUnitigNodes_list(const vector<u_int32_t>& unitigs, unordered_set<u_int32_t>& nodeIndexes){
        
        nodeIndexes.clear();

        for(u_int32_t unitigIndex : unitigs){
            vector<u_int32_t> nodes;
            getUnitigNodes(_unitigs[unitigIndex], nodes);
            for(u_int32_t nodeIndex : nodes){
                //_logFile << "Scc: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                nodeIndexes.insert(nodeIndex);
            }
        }

    }

    void getUnitigNodes(const Unitig& unitig, vector<u_int32_t>& nodes){


        nodes.clear();
        for(u_int32_t nodeIndex : unitig._nodes){
            nodes.push_back(nodeIndex);
        }

        return; // unitig._nodes;

        bool isCircular = false;

        nodes.clear();
        u_int32_t node = unitig._startNode;
        vector<u_int32_t> neighbors;


        while(true){
            //_logFile << node << endl;
            nodes.push_back(node);

            //_logFile << nodes.size() << " " << unitig._nbNodes << endl;
            if(nodes.size() == unitig._nbNodes) break;

            getSuccessors(node, 0, neighbors);
            //_logFile << neighbors.size() << endl;
            node = neighbors[0];
        }
        /*
            if(unitig._startNode == 162558){
                _logFile << "-------------" << endl;
                _logFile << unitig._startNode << " " << unitig._endNode << endl;
                getSuccessors(unitig._endNode, 0, neighbors);
                _logFile << "nb successors: " << neighbors.size() << endl;
                getPredecessors(unitig._startNode, 0, neighbors);
                _logFile << "nb predecessors: " << neighbors.size() << endl;
                exit(1);
            }

        getSuccessors(node, 0, neighbors);

        if(unitig._startNode == unitig._endNode && neighbors.size() == 1){
            isCircular = true;
        }

        size_t i = 0;
        while(true){

            //if(_isNodeRemoved[node]) continue;

            //u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(node, dummy);
            nodes.push_back(node);

            getSuccessors(node, 0, neighbors);
            //if(neighbors.size() != 1) break;

            if(isCircular){
                if(i == 0){ 
                    if(neighbors[0] == unitig._endNode) break; //Unitig with single node circular, or single node without edges
                }
                else{
                    if(node == unitig._endNode) break;
                }
            }
            else if(unitig._startNode == unitig._endNode){ //Single node circular and with several neighbors
                break;
            }
            else{
                if(node == unitig._endNode) break;
            }

            node = neighbors[0];

            i += 1;
        }

        */
    }

	double compute_median_float(vector<float> scores){
		size_t size = scores.size();

		if (size == 0){
			return 0;  // Undefined, really.
		}
		else{
			sort(scores.begin(), scores.end());
			if (size % 2 == 0){
				return (scores[size / 2 - 1] + scores[size / 2]) / 2; //scores[size / 2 - 1];
			}
			else {
				return scores[size / 2];
			}
		}
	}

    float _prevAbudanceCutoff;

    struct LongNeighbor{
        u_int32_t _nodeIndex;
        float _abundance;
    };

    vector<u_int32_t> sccDebug;
    ofstream file_debug;
    int __loadState2_index = 0;

    /*
    void loadState2(float abundanceCutoff_min, u_int32_t nodeIndex_source, const vector<UnitigData>& unitigDatas){

        //file_debug = ofstream("/home/gats/workspace/run//debug_graph.csv");
		//file_debug << "Name,Colour" << endl;

        //auto t1 = std::chrono::high_resolution_clock::now();

        _logFile << "Loading state: " << abundanceCutoff_min << endl;
        //vector<Bubble> bubbles;

        _logFile << "clear" << endl;
        clear(0);
        _logFile << "load" << endl;

        for(const SaveState2& saveState : _cachedGraphStates){
            //_logFile << saveState._abundanceCutoff_min << endl;
            if(saveState._abundanceCutoff_min > abundanceCutoff_min) break;
            //_logFile << "Applying state changes: " << saveState._abundanceCutoff_min << endl;

            for(const u_int32_t nodeName : saveState._nodeNameRemoved){

                u_int32_t nodeIndex1 = BiGraph::nodeName_to_nodeIndex(nodeName, true);
                u_int32_t nodeIndex2 = BiGraph::nodeName_to_nodeIndex(nodeName, false);

                _isNodeValid2.erase(nodeIndex1);
                _isNodeValid2.erase(nodeIndex2);
            }



            for(const DbgEdge& dbgEdge : saveState._isEdgeRemoved){
                _isEdgeRemoved.insert(dbgEdge);
            }


        }

        //Use connected component pour opti compact ????????????????????????????????????????????????????
        //if(useConnectComponent){
            //compact(true);
        //    compact(false);
        //    removeUnconnectedNodes(nodeIndex_source);
        //    compact(false);
        //}

        //std::reverse(bubbles.begin(), bubbles.end());
        
        _logFile << "compact" << endl;
        compact(false, unitigDatas);
        _logFile << "done" << endl;
        //removeUnconnectedNodes(nodeIndex_source);
        //compact(false);


        


        _currentUnitigNodes.clear();
        if(nodeIndex_source != -1){
            if(_isNodeValid2.find(nodeIndex_source) != _isNodeValid2.end()){
                for(u_int32_t nodeIndex : nodeIndex_to_unitig(nodeIndex_source)._nodes){
                    _currentUnitigNodes.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex));
                }
            }
        }




        //auto t2 = std::chrono::high_resolution_clock::now();
        //auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        //std::_logFile << ms_int.count() << "ms\n";
        //getchar();
    }
    */

    /*
    void detectBubbles(GraphSimplify* graphMain, const unordered_set<u_int32_t>& validNodeNames, u_int64_t maxLength, const vector<UnitigData>& unitigDatas, unordered_set<u_int32_t>& bubbleNodeNames){

        SaveState2 currentSaveState = {0, {}, {}, {}, {}};
        
        bubbleNodeNames.clear();

        compact(false, unitigDatas);
        

        while(true){

            vector<bool> isBubble = vector<bool>(_graphSuccessors->_nbNodes, false);
            bool isModSub = false;

            while(true){
                compact(true, unitigDatas);
                u_int64_t nbSuperbubblesRemoved = superbubble(maxLength, isBubble, currentSaveState, false, graphMain, false);
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb superbubble removed: " << nbSuperbubblesRemoved << endl;
                #endif
                if(nbSuperbubblesRemoved == 0) break;
                isModSub = true;
            }
            

            //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;

            while(true){
                compact(true, unitigDatas);
                u_int64_t nbBubblesRemoved = bubble(maxLength, currentSaveState, false, graphMain);
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb bubble removed: " << nbBubblesRemoved << endl;
                #endif
                if(nbBubblesRemoved == 0) break;
                isModSub = true;
            }

            if(!isModSub) break;

        }

        for(u_int32_t nodeName : validNodeNames){
            if(_isBubble[BiGraph::nodeName_to_nodeIndex(nodeName, false)]){
                bubbleNodeNames.insert(nodeName);
            }
        }
    }
    */

    unordered_set<u_int32_t> _possibleTips;
    bool __debug_lala = false;

    void debug_writeGfaErrorfree(u_int32_t currentAbundance, float abundanceCutoff_min, u_int32_t nodeIndex_source, u_int64_t k, bool saveGfa, bool doesSaveState, bool doesLoadState, const vector<UnitigData>& unitigDatas, bool crushBubble, bool smallBubbleOnly, bool detectRoundabout, bool insertBubble, bool saveAllState, bool doesSaveUnitigGraph, MDBG* mdbg, size_t minimizerSize, size_t nbCores, bool useLocalAbundanceFilter, bool removeLongTips){

        __debug_lala = false;
        file_debug = ofstream("/home/gats/workspace/run//debug_graph.csv");
		file_debug << "Name,Colour" << endl;
        /*
        u_int32_t targetNodeName = -1;
        if(mdbg != nullptr){
            for(auto& it : mdbg->_dbg_nodes){
                        
                for(size_t i=0; i<it.first._kmers.size()-2; i++){
                    if(it.first._kmers[i] == 4901087538459952 && it.first._kmers[i+1] == 14618695441502469 && it.first._kmers[i+2] == 79737352576542472){
                        targetNodeName = it.second._index;
                    }
                }
            }
        }
        */


        //_logFile << _graphSuccessors->_nodeDatas[BiGraph::nodeIndex_to_nodeName(894141)]._abundance << endl;
        //getchar();
        _logFile << "Cleaning graph" << endl;
        
        u_int64_t maxBubbleLength;
        if(smallBubbleOnly){
            maxBubbleLength = _kminmerSize * 2;
        }
        else{
            maxBubbleLength = _kminmerSize * 10;
        }

        maxBubbleLength = 50000;

        _cachedGraphStates.clear();

        SaveState2 currentSaveState = {0, {}, {}, {}, {}};

        _removedReadIndex.clear();



        unordered_map<u_int32_t, vector<LongNeighbor>> nodeLongNeighbors;
        bool useConnectComponent = nodeIndex_source != 0;
        /*
        if(abundanceCutoff_min < 6){
            useConnectComponent = false;
            if(((u_int64_t)_prevAbudanceCutoff) == ((u_int64_t)abundanceCutoff_min)) return;
            //useConnectComponent = true;
            //abundanceCutoff_min = 1;
        }*/
        
        //if(currentAbundance == _lastAbundanceCutoff) return;
        //_lastAbundanceCutoff = currentAbundance;
        
        //bool needCleaning = true;


        //loadState(abundanceCutoff_min);

        #ifdef PRINT_DEBUG_SIMPLIFICATION
            _logFile << "Start cleaning: " << saveGfa << " " << doesSaveState << " " << doesLoadState << endl;
        #endif

        clear(0);

        //UnitigFunctor functor(this, false);

        //functor(BiGraph::nodeName_to_nodeIndex(186593, true));
        //functor(BiGraph::nodeName_to_nodeIndex(159347, true));
        //functor(BiGraph::nodeName_to_nodeIndex(159346, true));
        //functor(BiGraph::nodeName_to_nodeIndex(159345, true));

        //exit(1);

        //u_int64_t nodeIndexSum = 0;
        //for(u_int32_t nodeIndex : _isNodeValid2){
        //    nodeIndexSum += nodeIndex;
        //}
        //_logFile << "Node index sum: " << nodeIndexSum << endl;

        compact(false, unitigDatas);

        //_unitigGraph->save("/home/gats/workspace/run/unitigGraph.gfa");
    
        
        return;
        
        /*
        currentAbundance = 0;
        abundanceCutoff_min = 0;
        for(const Unitig& u : _unitigs){
            if(u._abundance > abundanceCutoff_min){
                abundanceCutoff_min = u._abundance;
                currentAbundance = u._abundance;
            }
        }
        */
        

        _logFile << "Max abundance: " << abundanceCutoff_min << endl;
        /*
        u_int64_t nbErrorRemoved = removeErrors_3(k, 2000, unitigDatas, detectRoundabout, maxBubbleLength, insertBubble, saveAllState, doesSaveUnitigGraph, mdbg, minimizerSize, nbCores);
        
        //if(nbErrorRemoved > 0){
            compact(true, unitigDatas);
            resizeUnitigs();
            //isModification = true;
        //}
        */

        //nodeIndexSum = 0;
        //for(u_int32_t nodeIndex : _isNodeValid2){
        //    nodeIndexSum += nodeIndex;
        //}
        //_logFile << "Node index sum: " << nodeIndexSum << endl;

        /*
        bool isHere = false;
        for(u_int32_t nodeIndex : _isNodeValid2){
            if(BiGraph::nodeIndex_to_nodeName(nodeIndex) == targetNodeName){
                isHere = true;
                break;
            }
        }
        _logFile << "Is here 1: " << isHere << endl;
        _logFile << getNodeUnitigAbundance(BiGraph::nodeName_to_nodeIndex(targetNodeName, false)) << endl;
        */

        //removeErrors_3(k, 2000, unitigDatas, detectRoundabout, maxBubbleLength, insertBubble, saveAllState, doesSaveUnitigGraph, mdbg, minimizerSize, nbCores);
        //compact(true, unitigDatas);
        //resizeUnitigs();

        /*
        isHere = false;
        for(u_int32_t nodeIndex : _isNodeValid2){
            if(BiGraph::nodeIndex_to_nodeName(nodeIndex) == targetNodeName){
                isHere = true;
                break;
            }
        }
        _logFile << "Is here 2: " << isHere << endl;
        */

		//if(doesSaveUnitigGraph) saveUnitigGraph(_outputDir + "/minimizer_graph_u.gfa", mdbg, minimizerSize, nbCores, false);

        //vector<Bubble> bubbles;
        //vector<float> lala = {40, 800};
        //_logFile << compute_median_float(lala) << endl;
        //exit(1);
        /*
        for(u_int32_t nodeIndex : _isNodeValid2){
            
			vector<u_int32_t> successors;
			getSuccessors(nodeIndex, 0, successors);

			for(u_int32_t nodeIndex_successor : successors){

                vector<u_int32_t> predecessors;
                getPredecessors(nodeIndex_successor, 0, predecessors);

                if(std::find(predecessors.begin(), predecessors.end(), nodeIndex) == predecessors.end()){
                    _logFile << "Symetrical edge issue: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << " -> " << BiGraph::nodeIndex_to_nodeName(nodeIndex_successor) << endl;
                    getchar();
                }
            }

        }*/

        /*
        _logFile << "lala1" << endl;
        for(Unitig& unitig : _unitigs){
            if(unitig._startNode == -1) continue;
            if(unitigIndex_toReverseDirection(unitig._index) == -1){
                _logFile << "KC" << endl;
                getchar();
            }
            if(unitig._nbNodes != _unitigs[unitigIndex_toReverseDirection(unitig._index)]._nbNodes){
                _logFile << "KC" << endl;
                getchar();
            }
            //if(unitig._nbNodes != )
        }
        _logFile << "lala2" << endl;
        */

        /*
        for(u_int32_t nodeIndex : _isNodeValid2){

            if(395881 == BiGraph::nodeIndex_to_nodeName(nodeIndex)){
                _logFile << "lala 1" << endl;
                getchar();
            }
        }*/
        //clear(0);

        unordered_set<u_int64_t> rareReadsAll;
        _prevAbudanceCutoff = abundanceCutoff_min;
        float currentCutoff = 0;
        //long nbBubbleRoundRemaining = 10;
        //long nbBubbleRoundRemaining_current = nbBubbleRoundRemaining;

        while(true){


            //_logFile << std::fixed << "Init: " << getChecksumGlobal_utg() << " " << getChecksumGlobal_abundanceUtg() << endl;
            //getchar();

            bool firstBubblePass = true;
            bool firstTipPass = true;
            //compact(false, unitigDatas);
            //_logFile << _isNodeValid2.size() << " " << getChecksumGlobal_abundance() << " " << getChecksumGlobal_abundanceUtg() << endl;
            //getchar();

            u_int64_t nbTipsRemoved = 0;
            //u_int64_t nbErrorRemoved = 0;
            u_int64_t nbBubblesRemoved = 0;
            u_int64_t nbSmallLoopRemoved = 0;

            bool isModification = false;
            bool isModSub = false;





            while(true){

                isModSub = false;
                
                /*
                while(true){
                    compact(true, unitigDatas);

                    u_int64_t nbSelfLoopRemoved = removeSelfLoops(currentSaveState);

                    #ifdef PRINT_DEBUG_SIMPLIFICATION
                        _logFile << "Nb self loop removed: " << nbSelfLoopRemoved << endl;
                    #endif
                    if(nbSelfLoopRemoved == 0) break;
                    isModification = true;
                    isModSub = true;

                    
                }
                */
                
                
                
                //3526895 2681795
                //if(crushBubble && nbBubbleRoundRemaining_current > 0){
                //    nbBubbleRoundRemaining_current -= 1;

                    
                    while(true){

                        //if(_unitigIndexToClean.size() > 100000){
                        //    compact(false, unitigDatas);
                        //}

                        bool isModBubble = false;
                        
                        vector<bool> isBubble = vector<bool>(_graphSuccessors->_nbNodes, false);
                        
                        __debug_lala = false;

                        while(true){

                            _logFile << "superbubble" << endl;
                            compact(true, unitigDatas);
                            //_logFile << "1: " << _unitigIndexToClean.size() << endl;
                            u_int64_t nbSuperbubblesRemoved = superbubble(maxBubbleLength, isBubble, currentSaveState, false, nullptr, false);
                            #ifdef PRINT_DEBUG_SIMPLIFICATION
                                _logFile << "Nb superbubble removed: " << nbSuperbubblesRemoved << endl;
                            #endif
                            if(nbSuperbubblesRemoved == 0) break;
                            isModification = true;
                            isModSub = true;
                            isModBubble = true;


                        }
                        //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;
                        
                        //_logFile << "Superbubble: " << getChecksumGlobal_utg() << " " << getChecksumGlobal_abundanceUtg() << endl;
                        //getchar();
                        //_logFile << "Superbubble: " << getChecksumGlobal() << endl;
                        //getchar();
                        
                        isBubble = vector<bool>(_graphSuccessors->_nbNodes, false);
                        __debug_lala = true;

                        while(true){

                            _logFile << "bubble" << endl;

                            compact(true, unitigDatas);
                            //getchar();
                            //_logFile << "2: " << _unitigIndexToClean.size() << endl;
                            nbBubblesRemoved = bubble(maxBubbleLength, isBubble, currentSaveState, false, nullptr);
                            #ifdef PRINT_DEBUG_SIMPLIFICATION
                                _logFile << "Nb bubble removed: " << nbBubblesRemoved << endl;
                            #endif
                            if(nbBubblesRemoved == 0) break;
                            isModification = true;
                            isModSub = true;
                            isModBubble = true;
                        }
                        
                        
                        //_logFile << "Bubble: " << getChecksumGlobal_utg() << " " << getChecksumGlobal_abundanceUtg() << endl;

                        //getchar();

                        //_logFile << "bubble: " << getChecksumGlobal() << endl;
                        //_logFile << nodeIndex_to_unitig(BiGraph::nodeName_to_nodeIndex(22716, true))._nbNodes << endl;
                        //getchar();

                        if(!isModBubble) break;
                    }
                    

                    if(firstBubblePass){
                        firstBubblePass = false;
                        compact(true, unitigDatas);
                        resizeUnitigs();
                    }



                    /*
                    tip(50000, false, currentSaveState, false, true, false, currentCutoff);
                    
                    float localCutoff = 1;
                    //float aplha = 0.1;

                    for(size_t i=0; i<maxAbundance; i++){
                        while(true){

                            bool isTipRemoved = false;

                            while(true){
                                compact(true, unitigDatas);
                                u_int64_t nbRemoved = tip(_kminmerSize, false, currentSaveState, true, false, false, localCutoff);
                                #ifdef PRINT_DEBUG_SIMPLIFICATION
                                    _logFile << "Nb tip removed: " << nbRemoved << endl;
                                #endif
                                if(nbRemoved > 0){
                                    isModification = true;
                                    isModSub = true;
                                    isTipRemoved = true;
                                }
                                else{
                                    break;
                                }
                            }


                            //_logFile << "Tip 1: " << getChecksumGlobal_utg() << " " << getChecksumGlobal_abundanceUtg() << endl;
                            //getchar();

                            //_logFile << "Tip 1: " << getChecksumGlobal() << " " << _tip_checksum << endl;
                            //getchar();

                            if(isTipRemoved) continue;

                            while(true){
                                compact(true, unitigDatas);
                                u_int64_t nbRemoved = tip(_kminmerSize*2, false, currentSaveState, true, false, false, localCutoff);
                                #ifdef PRINT_DEBUG_SIMPLIFICATION
                                    _logFile << "Nb tip removed: " << nbRemoved << endl;
                                #endif
                                if(nbRemoved > 0){
                                    isModification = true;
                                    isModSub = true;
                                    isTipRemoved = true;
                                }
                                else{
                                    break;
                                }
                            }

                            //_logFile << "Tip 2: " << getChecksumGlobal_utg() << " " << getChecksumGlobal_abundanceUtg() << endl;
                            //getchar();
                            //_logFile << "Tip 2: " << getChecksumGlobal() << " " << _tip_checksum << endl;
                            //getchar();

                            if(isTipRemoved) continue;

                            while(true){
                                compact(true, unitigDatas);
                                u_int64_t nbRemoved = tip(50000, false, currentSaveState, false, false, false, localCutoff);
                                #ifdef PRINT_DEBUG_SIMPLIFICATION
                                    _logFile << "Nb tip removed: " << nbRemoved << endl;
                                #endif
                                if(nbRemoved > 0){
                                    isModification = true;
                                    isModSub = true;
                                    isTipRemoved = true;
                                }
                                else{
                                    break;
                                }
                            }


                            //_logFile << "Tip 3: " << getChecksumGlobal_utg() << " " << getChecksumGlobal_abundanceUtg() << endl;
                            //getchar();
                            //_logFile << "Tip 3: " << getChecksumGlobal() << " " << _tip_checksum << " " << _unitigs.size() << endl;
                            //getchar();

                            if(!isTipRemoved) break;
                        }

                        localCutoff = localCutoff * (1+aplha);
                    }
                    */

                    
                    tip(50000, false, currentSaveState, false, true, false);
                    
                    
                    while(true){

                        bool isTipRemoved = false;

                        while(true){
                            compact(true, unitigDatas);
                            u_int64_t nbRemoved = tip(_kminmerSize, false, currentSaveState, true, false, false);
                            #ifdef PRINT_DEBUG_SIMPLIFICATION
                                _logFile << "Nb tip removed: " << nbRemoved << endl;
                            #endif
                            if(nbRemoved > 0){
                                isModification = true;
                                isModSub = true;
                                isTipRemoved = true;
                            }
                            else{
                                break;
                            }
                        }


                        //_logFile << "Tip 1: " << getChecksumGlobal_utg() << " " << getChecksumGlobal_abundanceUtg() << endl;
                        //getchar();

                        //_logFile << "Tip 1: " << getChecksumGlobal() << " " << _tip_checksum << endl;
                        //getchar();

                        if(isTipRemoved) continue;

                        while(true){
                            compact(true, unitigDatas);
                            u_int64_t nbRemoved = tip(_kminmerSize*2, false, currentSaveState, true, false, false);
                            #ifdef PRINT_DEBUG_SIMPLIFICATION
                                _logFile << "Nb tip removed: " << nbRemoved << endl;
                            #endif
                            if(nbRemoved > 0){
                                isModification = true;
                                isModSub = true;
                                isTipRemoved = true;
                            }
                            else{
                                break;
                            }
                        }

                        //_logFile << "Tip 2: " << getChecksumGlobal_utg() << " " << getChecksumGlobal_abundanceUtg() << endl;
                        //getchar();
                        //_logFile << "Tip 2: " << getChecksumGlobal() << " " << _tip_checksum << endl;
                        //getchar();

                        if(isTipRemoved) continue;

                        while(true){
                            compact(true, unitigDatas);
                            u_int64_t nbRemoved = tip(50000, false, currentSaveState, false, false, false);
                            #ifdef PRINT_DEBUG_SIMPLIFICATION
                                _logFile << "Nb tip removed: " << nbRemoved << endl;
                            #endif
                            if(nbRemoved > 0){
                                isModification = true;
                                isModSub = true;
                                isTipRemoved = true;
                            }
                            else{
                                break;
                            }
                        }


                        //_logFile << "Tip 3: " << getChecksumGlobal_utg() << " " << getChecksumGlobal_abundanceUtg() << endl;
                        //getchar();
                        //_logFile << "Tip 3: " << getChecksumGlobal() << " " << _tip_checksum << " " << _unitigs.size() << endl;
                        //getchar();

                        if(!isTipRemoved) break;
                    }
                    
                    //getchar();
                    
                    /*
                    while(true){
                        
                        //compact(true, unitigDatas);
                        //_logFile << "3: " << _unitigIndexToClean.size() << endl;

                        //u_int64_t nbRemovedTotal = 0;

                        //unordered_set<u_int32_t> isTips;
                        //nbTipsRemoved = tip(4*k, true, isTips);
                        //nbTipsRemoved = tip(4*k, false, isTips);
                        //nbTipsRemoved = tip(50000, true, isTips, currentSaveState, removeLongTips);
                            
                        //while(true){
                            u_int64_t nbRemoved = 0;
                            for(u_int64_t maxLength : {1000, 5000, 10000, 15000, 20000, 25000, 30000, 35000, 40000, 45000, 50000}){
                                //_logFile << "tip " << maxLength << endl;
                                compact(true, unitigDatas);
                                nbRemoved = tip(maxLength, false, currentSaveState, false, false, false);
                                //nbRemovedTotal += nbRemoved;
                                if(nbRemoved > 0) break;
                            }
                            if(nbRemoved == 0) break;
                        //}

                        #ifdef PRINT_DEBUG_SIMPLIFICATION
                            _logFile << "Nb tip removed: " << nbRemovedTotal << endl;
                        #endif

                        if(nbRemoved > 0){
                            isModification = true;
                            isModSub = true;
                        }
                        else{
                            break;
                        }
                    }
                    */


                    if(firstTipPass){
                        firstTipPass = false;
                        compact(true, unitigDatas);
                        resizeUnitigs();
                    }
                    
                    //resizeUnitigs();
                    
                    

                    
                    

                    //_logFile << "4: " << _unitigIndexToClean.size() << endl;
                    //if(nbTipsRemoved == 0) break;
                    //isModification = true;
                    //isModSub = true;

                //}

                //_logFile << _isBubble[BiGraph::nodeName_to_nodeIndex(510153, true)] << " " << _isBubble[BiGraph::nodeName_to_nodeIndex(510153, false)] << endl;
                //_logFile << _isBubble[BiGraph::nodeName_to_nodeIndex(1718667, true)] << " " << _isBubble[BiGraph::nodeName_to_nodeIndex(1718667, false)] << endl;
                //_logFile << getNodeUnitigAbundance(BiGraph::nodeName_to_nodeIndex(1473953, true)) << endl;
                //_logFile << getNodeUnitigAbundance(BiGraph::nodeName_to_nodeIndex(3541558, true)) << endl;
                
                /*
                while(true){
                    compact(true, unitigDatas);
                    nbSmallLoopRemoved = removeSmallLoop(10000, currentSaveState);
                    #ifdef PRINT_DEBUG_SIMPLIFICATION
                        _logFile << "Nb small loop removed: " << nbSmallLoopRemoved << endl;
                    #endif
                    if(nbSmallLoopRemoved == 0) break;
                    isModification = true;
                    isModSub = true;
                }
                */
                
                
                /*
                _logFile << "detecting readpath 1" << endl;
                while(true){
                    compact(true, unitigDatas);
                    u_int64_t nbSuperbubblesRemoved = superbubble(50000, isBubble, currentSaveState, true);
                    #ifdef PRINT_DEBUG_SIMPLIFICATION
                        _logFile << "Nb superbubble removed: " << nbSuperbubblesRemoved << endl;
                    #endif
                    if(nbSuperbubblesRemoved == 0) break;
                    isModification = true;
                    isModSub = true;
                }
                
                _logFile << "detecting readpath 2" << endl;

                //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;

                while(true){
                    compact(true, unitigDatas);
                    nbBubblesRemoved = bubble(50000, currentSaveState, true);
                    #ifdef PRINT_DEBUG_SIMPLIFICATION
                        _logFile << "Nb bubble removed: " << nbBubblesRemoved << endl;
                    #endif
                    if(nbBubblesRemoved == 0) break;
                    isModification = true;
                    isModSub = true;
                }

                
                _logFile << "detecting readpath 3" << endl;
                */

                //compact(false);
                //_logFile << nodeIndex_to_unitigIndex(BiGraph::nodeName_to_nodeIndex(792165, true)) << endl;
                //getStronglyConnectedComponent(nodeIndex_to_unitigIndex(BiGraph::nodeName_to_nodeIndex(792165, true)), true, sccDebug);
                //_logFile << sccDebug.size() << endl;


                if(!isModSub) break;


            }
        


            //if(doesSaveState){
            //}

            
            //if(useLocalAbundanceFilter){

            //    if(_kminmerSize == 4){
                    //removeErrors_4(k, abundanceCutoff_min, currentCutoff, currentSaveState, unitigDatas, detectRoundabout, maxBubbleLength, insertBubble, saveAllState, doesSaveUnitigGraph, mdbg, minimizerSize, nbCores);
                
            //    }
                
            //}
            //else{
            checkSaveState(currentCutoff, unitigDatas, detectRoundabout, maxBubbleLength, currentSaveState, insertBubble, doesSaveUnitigGraph, mdbg, minimizerSize, nbCores);
        
            u_int64_t nbErrorRemoved = removeErrors_2(k, abundanceCutoff_min, currentCutoff, currentSaveState, unitigDatas, detectRoundabout, maxBubbleLength, insertBubble, saveAllState, doesSaveUnitigGraph, mdbg, minimizerSize, nbCores);
        
            compact(true, unitigDatas);
            resizeUnitigs();    

            if(nbErrorRemoved > 0){
                //compact(true, unitigDatas);
                //resizeUnitigs();
                //nbBubbleRoundRemaining_current = nbBubbleRoundRemaining;
                isModification = true;
            }
            //}
            

 
           

            //
            //    saveState();
            //}

            //_logFile << "Error: " << _isNodeRemoved[loulou] << endl;
            //compact();
            



            //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;

            //exit(1);






            /*
            unordered_set<u_int32_t> validNodes;
            for (auto& nodeIndex : _isNodeValid2){
                u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
                validNodes.insert(nodeName);
            }
            GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, _inputGfaFilename + "_errorFree.gfa", validNodes, _isEdgeRemoved, _graphSuccessors);
            */

            





            if(!isModification) break;
        
        }
 
        //if(useLocalAbundanceFilter){
        //    checkSaveState(0, unitigDatas, detectRoundabout, maxBubbleLength, currentSaveState, insertBubble, doesSaveUnitigGraph, mdbg, minimizerSize, nbCores);
        //}

        //compact(false, unitigDatas);

        //compact(true, unitigDatas);  
        //resizeUnitigs();  

        //saveUnitigGraph(_outputDir + "/minimizer_graph_u_cleaned.gfa", mdbg, minimizerSize, nbCores, true);

        /* !!!

        unordered_set<u_int32_t> validNodes;
        for (auto& nodeIndex : _isNodeValid2){
            u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
            validNodes.insert(nodeName);
        }
        string outputFilename = _outputDir + "/minimizer_graph_sub.gfa";
        GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, outputFilename, validNodes, _isEdgeRemoved, _graphSuccessors);
        */

        //_logFile << "Nb graph state: " << _cachedGraphStates.size() << endl;
        

        /*
        for(size_t nodeIndex=0; nodeIndex<_graphSuccessors->_nbNodes; nodeIndex++){
            if(_isNodeValid2.find(nodeIndex) != _isNodeValid2.end()) continue;
            u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(nodeIndex);
            const UnitigData& unitigData = unitigDatas[nodeName];
            for(u_int64_t readIndex : unitigData._readIndexes){
                _removedReadIndex.insert(readIndex);
            }
        }*/

        //_logFile << "Nb invalid reads: " << _removedReadIndex.size() << endl;
        /*
        for(u_int32_t nodeIndex : _isNodeValid2){
            if(_isNodeValid2.find(nodeIndex_toReverseDirection(nodeIndex)) == _isNodeValid2.end()){
                _logFile << "omg" << endl;
                getchar();
            }
        }*/

        /*
        //if(currentAbundance == _lastAbundanceCutoff) return;
        //_lastAbundanceCutoff = currentAbundance;
        
        //bool needCleaning = true;
        bool useConnectComponent = true;
        if(abundanceCutoff_min < 6){
            useConnectComponent = false;
            if(((u_int64_t)_prevAbudanceCutoff) == ((u_int64_t)abundanceCutoff_min)) return;
            //useConnectComponent = true;
            //abundanceCutoff_min = 1;
        }

        clear(abundanceCutoff_min);
		//loadState();



        _prevAbudanceCutoff = abundanceCutoff_min;

        while(true){

            u_int64_t nbTipsRemoved = 0;
            u_int64_t nbErrorRemoved = 0;
            u_int64_t nbBubblesRemoved = 0;
            u_int64_t nbSmallLoopRemoved = 0;

            bool isModification = false;

            //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;

            if(useConnectComponent){
                compact();
                removeUnconnectedNodes(nodeIndex_source);
            }

            //u_int32_t loulou = BiGraph::nodeName_to_nodeIndex(214, false);

            //remove very low abundant nodes
            while(true){
                compact();
                nbErrorRemoved = removeLowAbundantNodes(abundanceCutoff_min/2.0);
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb error removed 1: " << nbErrorRemoved << endl;
                #endif
                if(nbErrorRemoved == 0) break;
                isModification = true;
            }

            //Remove abundant nodes progressively (should save variant bubbles with lower abundance)
            for(float i=0.1; i<abundanceCutoff_min; i++){
                while(true){
                    compact();
                    nbErrorRemoved = removeLowAbundantNodes(i);
                    #ifdef PRINT_DEBUG_SIMPLIFICATION
                        _logFile << "Nb error removed 2: " << nbErrorRemoved << endl;
                    #endif
                    if(nbErrorRemoved == 0) break;
                    isModification = true;
                }
            }

            //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;

            if(useConnectComponent){
                removeUnconnectedNodes(nodeIndex_source);
            }
            getStronglyConnectedComponent_node
            //_logFile << "Error: " << _isNodeRemoved[loulou] << endl;
            //compact();

            while(true){
                compact();
                nbTipsRemoved = tip(50000);
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb tip removed: " << nbTipsRemoved << endl;
                #endif
                if(nbTipsRemoved == 0) break;
                isModification = true;
            }

            //_logFile << "tip: " << _isNodeRemoved[loulou] << endl;

            

            //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;
            
            
            while(true){
                compact();
                u_int64_t nbSuperbubblesRemoved = superbubble(50000);
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb superbubble removed: " << nbSuperbubblesRemoved << endl;
                #endif
                if(nbSuperbubblesRemoved == 0) break;
                isModification = true;
            }
            nodeIndex_neighbor
            //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;

            while(true){
                compact();
                nbBubblesRemoved = bubble(50000);
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb bubble removed: " << nbBubblesRemoved << endl;
                #endif
                if(nbBubblesRemoved == 0) break;
                isModification = true;
            }

            //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;

            //exit(1);
            while(true){
                compact();
                nbSmallLoopRemoved = removeSmallLoop(10000);
                #ifdef PRINT_DEBUG_SIMPLIFICATION
                    _logFile << "Nb small loop removed: " << nbSmallLoopRemoved << endl;
                #endif
                if(nbSmallLoopRemoved == 0) break;
                isModification = true;
            }

            //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;

            //_logFile << "Small bubble: " << _isNodeRemoved[loulou] << endl;

            if(!isModification) break;
        }


        //_logFile << "Nb bubbles: " << _bubbles.size() << endl;
        //for(Bubble& bubble : _bubbles){
        //    for(u_int32_t nodeIndex : bubble._nodes){
        //        _logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
        //    }
        //}

        while(true){
            vector<u_int32_t> neighbors;
            u_int64_t bubbleAdded = 0;

            for(Bubble& bubble : _bubbles){
                u_int32_t startNode = bubble._nodes[0];
                u_int32_t endNode = bubble._nodes[bubble._nodes.size()-1];
                if(_isNodeValid2.find(startNode) != _isNodeValid2.end()) continue;
                //if(_isNodeValid[startNode]) continue; //&& !_isNodeRemoved[endNode]

                getPredecessors(startNode, 0, neighbors);
                if(neighbors.size() == 0) continue;

                bool isPathValid = false;
                for(u_int32_t n : neighbors){
                    //if(_isNodeValid[n]){
                    if(_isNodeValid2.find(n) != _isNodeValid2.end()){
                        isPathValid = true;
                        break;
                    }
                }

                if(!isPathValid) continue;

                getSuccessors(endNode, 0, neighbors);
                if(neighbors.size() == 0) continue;

                isPathValid = false;
                for(u_int32_t n : neighbors){
                    if(_isNodeValid2.find(n) != _isNodeValid2.end()){
                    //if(_isNodeValid[n]){
                        isPathValid = true;
                        break;
                    }
                }

                if(!isPathValid) continue;

                for(u_int32_t nodeIndex : bubble._nodes){
                    _isNodeValid2.insert(nodeIndex);
                    _isNodeValid2.insert(nodeIndex_toReverseDirection(nodeIndex));
                    //_logFile << "Bubble saved: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;
                    //_isNodeValid[nodeIndex] = true;
                }

                bubbleAdded += 1;
            }

            if(bubbleAdded == 0) break;
        }

        compact();

        //_logFile << (_isNodeValid2.find(BiGraph::nodeName_to_nodeIndex(1840, false)) != _isNodeValid2.end())<< endl;
        _logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;
        //compact();
        //superbubble(50000);
        */
        
        /*
        //Debug gfa
        unordered_set<u_int32_t> validNodes;
        for (auto& nodeIndex : _isNodeValid2){
            u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
            validNodes.insert(nodeName);
        }
        GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, _inputGfaFilename + "_errorFree.gfa", validNodes, _isEdgeRemoved, _graphSuccessors);
        */

        //exit(1);
        /*
        //_logFile << "orlolo" << endl;
        for(size_t nodeIndex=0; nodeIndex<_graphSuccessors->_nbNodes; nodeIndex++){
            //_logFile << nodeIndex << " " << BiGraph::nodeName_to_nodeIndex(214, true) << endl;
            //if(nodeIndex == BiGraph::nodeName_to_nodeIndex(214, true)){
            //    _logFile << "haha" << endl;
            //    _logFile << _isNodeRemoved[nodeIndex] << endl;
            //}
            //if(nodeIndex == BiGraph::nodeName_to_nodeIndex(214, false)){
            //    _logFile << "haha" << endl;
            //    _logFile << _isNodeRemoved[nodeIndex] << endl;
            //}

            //if(_isNodeValid[nodeIndex]) continue;
            if(_isNodeValid2.find(nodeIndex) != _isNodeValid2.end()){
                _logFile << _graphSuccessors->nodeIndex_to_nodeName(nodeIndex) << endl;
                _logFile << "lala" << _isNodeValid2.size() << endl;
                continue;
            }
            u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
            removedNodes.insert(nodeName);
        }
        */



        //_logFile << "lalalal" << endl;
        //_logFile << _isNodeRemoved[BiGraph::nodeName_to_nodeIndex(214, true)] << endl;

        //exit(1);

        _logFile << "Cleaning graph done" << endl;
        _logFile << "Check sum (superbubble): " << _superbubble_checksum << endl;
        _logFile << "Check sum (tip): " << _tip_checksum << endl;
        _logFile << "Check sum (ab cutoff): " << _abcutoff_checksum << endl;
        
        //getchar();

    }

    void resizeUnitigs(){
        
        _logFile << "resizing" << endl;
        _nextUnitigIndex = 0;
        vector<Unitig> unitigs;
        
        for(Unitig u : _unitigs){
            if(u._startNode == -1) continue;

            u_int32_t prevIndex = u._index;
            u._index = _nextUnitigIndex;
            unitigs.push_back(u);

            for(u_int32_t nodeIndex : u._nodes){
                _nodeToUnitig[nodeIndex] = u._index;
            }

            if(_possibleTips.find(prevIndex) != _possibleTips.end()){
                _possibleTips.erase(prevIndex);
                _possibleTips.insert(u._index);
            }

            _nextUnitigIndex += 1;
        }

        _unitigs = unitigs;

        _logFile << "done" << endl;
    }

    void checkSaveState(float currentCutoff, const vector<UnitigData>& unitigDatas, bool detectRoundabout, u_int64_t maxBubbleLength, SaveState2& currentSaveState, bool insertBubble, bool doesSaveUnitigGraph, MDBG* mdbg, size_t minimizerSize, size_t nbCores){
        
        bool saveStateExist = false;
        for(const SaveState2& saveState : _cachedGraphStates){
            if(currentCutoff == saveState._abundanceCutoff_min){
                saveStateExist = true;
                break;
            }
        }

        //_logFile << saveStateExist << endl;
        //_logFile << saveStateExist << " " << _cachedGraphStates.size() << endl;
        //getchar();

        //getchar();
        if(!saveStateExist){

            cout << "saving state: " << currentCutoff << endl;

            //getchar();
            compact(true, unitigDatas);
            if(doesSaveUnitigGraph && currentCutoff == 0) saveUnitigGraph(_outputDir + "/minimizer_graph_u_cleaned.gfa", mdbg, minimizerSize, nbCores, true);
            /*
            compact(false, unitigDatas);

            if(currentCutoff == 0){

                if(detectRoundabout){
                    //detectRoundabouts(maxBubbleLength, unitigDatas);
                }
                
                if(doesSaveUnitigGraph) saveUnitigGraph(_outputDir + "/minimizer_graph_u_cleaned.gfa", mdbg, minimizerSize, nbCores, true);
                //if(doesSaveUnitigGraph) saveGraph(_outputDir + "/minimizer_graph_cleaned.gfa");
		        collectStartingUnitigs(_kminmerSize);
            }



            unordered_set<u_int32_t> isNodeValid2_memo = _isNodeValid2;
            std::reverse(_bubbles.begin(), _bubbles.end());


            
            
            ///---------------------------- Inserting bubbles
            if(insertBubble){
                _logFile << "inserting bubbles" << endl;
            #ifdef PRINT_DEBUG_SIMPLIFICATION
                _logFile << "inserting bubbles" << endl;
            #endif

            unordered_set<u_int32_t> invalidBubbleNodes;
            unordered_set<u_int32_t> addedNodeNames;

            while(true){
                u_int64_t bubbleAdded = 0;

                
                for(const Bubble& bubble : _bubbles){
                    if(bubble._nodes.size() == 0){
                        continue;
                    }

                    u_int32_t startNode = bubble._startNode;
                    u_int32_t endNode = bubble._endNode;

                    if(_isNodeValid2.find(startNode) == _isNodeValid2.end()) continue;
                    if(_isNodeValid2.find(endNode) == _isNodeValid2.end()) continue;



                    vector<u_int32_t> addedNodes;

                    for(const NodeAbundance& node : bubble._nodes){

                        if(invalidBubbleNodes.find(node._nodeIndex) != invalidBubbleNodes.end()) continue;
                        //u_int32_t unitigIndex = nodeIndex_to_unitigIndex(nodeIndex);
                        //double localabundance = computeLocalAbundance(startUnitigIndex, k, nodeLongNeighbors);
                        //_logFile << localabundance << endl;
                        double cutoff = currentCutoff; //_globalCutoff-((float)k/6.0); //localabundance*0.5;

                        
                        //_logFile << "2" << endl;
                        if(node._abundance >= cutoff){
                            if(_isNodeValid2.find(node._nodeIndex) == _isNodeValid2.end()){
                                bubbleAdded += 1;
                                _isNodeValid2.insert(node._nodeIndex);
                                addedNodeNames.insert(BiGraph::nodeIndex_to_nodeName(node._nodeIndex));
                                addedNodes.push_back(node._nodeIndex);
                            }

                        }

                    }


                    while(true){ //Superbubble may contains nodes which pass the abundance filter and some others not, we remove tips created by this effect 
                        bool isModification = false;
                        
                        vector<u_int32_t> removedNodes;

                        for(u_int32_t nodeIndex : addedNodes){

                            vector<u_int32_t> neighbors;
                            getSuccessors(nodeIndex, 0, neighbors);
                            if(neighbors.size() == 0){
                                removedNodes.push_back(nodeIndex);
                                continue;
                            }

                            getPredecessors(nodeIndex, 0, neighbors);
                            if(neighbors.size() == 0){
                                removedNodes.push_back(nodeIndex);
                                continue;
                            }

                        }

                        for(u_int32_t nodeIndex : removedNodes){
                            //_logFile << "removed final tips: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;;
                            _isNodeValid2.erase(nodeIndex);
                            addedNodes.erase(std::remove(addedNodes.begin(), addedNodes.end(), nodeIndex), addedNodes.end());
                            invalidBubbleNodes.insert(nodeIndex);
                            if(addedNodeNames.find(BiGraph::nodeIndex_to_nodeName(nodeIndex)) != addedNodeNames.end()){
                                addedNodeNames.erase(BiGraph::nodeIndex_to_nodeName(nodeIndex));
                            }
                            isModification = true;
                        }

                        if(!isModification) break;
                    }

                }

                _logFile << "Bubble added: " << bubbleAdded << endl;
                if(bubbleAdded == 0) break;
            }
            
            for(u_int32_t nodeName : addedNodeNames){
                currentSaveState._nodeNameRemoved_tmp.erase(nodeName);
                //currentSaveState._nodeNameRemoved.erase(std::remove(currentSaveState._nodeNameRemoved.begin(), currentSaveState._nodeNameRemoved.end(), nodeName), currentSaveState._nodeNameRemoved.end());
            }
            //------------------------------ End
            }
            
            */
            

            for(u_int32_t nodeName : currentSaveState._nodeNameRemoved_tmp){
                currentSaveState._nodeNameRemoved.push_back(nodeName);
            }
            currentSaveState._nodeNameRemoved_tmp.clear();
            //getchar();

            //compact(false);

            /*
            unordered_set<u_int32_t> validNodes;
            for (auto& nodeIndex : _isNodeValid2){
                u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
                validNodes.insert(nodeName);
            }
            string outputFilename = _outputDir + "/minimizer_graph_sub.gfa";
            GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, outputFilename, validNodes, _isEdgeRemoved, _graphSuccessors);
                

            while(true){
                compact(true);

                unordered_set<u_int32_t> isTips;
                //nbTipsRemoved = tip(4*k, true, isTips);
                //nbTipsRemoved = tip(4*k, false, isTips);
                SaveState2 saveStateDummy;
                u_int64_t nbTipsRemoved = tip(50000, true, isTips, saveStateDummy);
                nbTipsRemoved = tip(50000, false, isTips, saveStateDummy);

                _logFile << "Final tips removed: " << nbTipsRemoved << endl;
                
                if(nbTipsRemoved == 0) break;
            }
            compact(false);
            getchar();
            */

            //_logFile << (_isNodeValid2.find(BiGraph::nodeName_to_nodeIndex(1840, false)) != _isNodeValid2.end())<< endl;
            //_logFile << "Nb nodes valid: " << _isNodeValid2.size() << endl;
            //getchar();

            /*
            file_debug.close();
            if(saveGfa){
                unordered_set<u_int32_t> validNodes;
                for (auto& nodeIndex : _isNodeValid2){
                    u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
                    validNodes.insert(nodeName);
                }
                string outputFilename = _outputDir + "/minimizer_graph_cleaned.gfa";
                GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, outputFilename, validNodes, _isEdgeRemoved, _graphSuccessors);
            }
            else{
                unordered_set<u_int32_t> validNodes;
                for (auto& nodeIndex : _isNodeValid2){
                    u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
                    validNodes.insert(nodeName);
                }
                string outputFilename = _outputDir + "/minimizer_graph_sub.gfa";
                GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, outputFilename, validNodes, _isEdgeRemoved, _graphSuccessors);
                
                
            }*/

            //compact();
            //superbubble(50000);
            
            //for(u_int32_t nodeIndex : isLongUnitigNodes){
            //    u_int32_t unitigIndex = nodeIndex_to_unitigIndex(nodeIndex);
            //    _isLongUnitig.insert(unitigIndex);
            //}

            /*
            unordered_set<u_int64_t> rareReads;
            u_int64_t nbRareUnitigs = 0;
            for(const Unitig& unitig : _unitigs){
                if(unitig._startNode == -1) continue;
                
                if(unitig._length > 4000 && unitig._abundance < 6){
                    nbRareUnitigs += 1;

                    for(u_int32_t nodeIndex : unitig._nodes){
                        for(u_int64_t readIndex : unitigDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex)]._readIndexes){
                            if(rareReadsAll.find(readIndex) != rareReadsAll.end()) continue;
                            rareReads.insert(readIndex);
                            rareReadsAll.insert(readIndex);
                        }
                    }
                    //for(u_int64_t readIndex : _unitigDatas2[unitig._index]._readIndexes){
                    //    if(rareReadsAll.find(readIndex) != rareReadsAll.end()) continue;
                    //    rareReads.insert(readIndex);
                    //    rareReadsAll.insert(readIndex);
                    //}
                    
                }
            }
            _logFile << "Rare unitigs: " << nbRareUnitigs << " / " << _unitigs.size() << endl;
            _logFile << "Rare reads: " << rareReadsAll.size() << endl;

            for(u_int64_t readIndex : rareReads){
                currentSaveState._rareReads.push_back(readIndex);
            }
            */

            currentSaveState._abundanceCutoff_min = currentCutoff;
            _cachedGraphStates.push_back(currentSaveState);
            currentSaveState = {0, {}, {}, {}, {}};
            //_logFile << "insert graph state" << currentCutoff << endl;

            /*
            if(insertBubble){
                _isNodeValid2 = isNodeValid2_memo; //!
            }
            std::reverse(_bubbles.begin(), _bubbles.end());
            compact(false, unitigDatas);

            unordered_set<u_int32_t> indexedNodeIndex;
            u_int64_t nodeIndexSum = 0;
            u_int32_t nbNodes = 0;
            double abTotal = 0;
            for(auto it: _unitigs){
                Unitig u = it.second;
                abTotal += u._abundance;
                nbNodes += u._nbNodes;

                unordered_map<u_int32_t, u_int32_t> nodelala;
                for(u_int32_t nodeIndex : u._nodes){
                    nodelala[nodeIndex] += 1;
                }

            }

            u_int64_t nodeIndexSum2 = 0;

            u_int64_t nodeIndexSum3 = 0;
            for(u_int32_t nodeIndex : indexedNodeIndex){
                nodeIndexSum3 += nodeIndex;
            }
            _logFile << "Save state: " << currentCutoff << endl;
            _logFile << std::fixed << "\tNb unitigs: " << _unitigs.size() << " " << nbNodes << " " << nodeIndexSum << " " << nodeIndexSum3 << " " << nodeIndexSum2 << " " << indexedNodeIndex.size() << " " << _isNodeValid2.size() << endl;
            _logFile << std::fixed << "\tAb total: " << abTotal << endl;
            _logFile << getChecksumGlobal_utg() << endl;
            //getchar();
            */
            

        
        }



        file_debug.close();
    }

    void collectStartingUnitigs(u_int64_t k){


        for(const Unitig& unitig: _unitigs){
            //_logFile << unitig._length << " " << unitig._abundance << endl;
            if(unitig._index % 2 == 1) continue;
            //if(unitig._length < 10000) continue;
            if(unitig._nbNodes < k*2) continue;
            //_logFileif(unitig._length < 50000) continue;
            //if(unitig._length < 10000 || unitig._length > 15000) continue;
            //if(unitig._abundance < 10) continue; //200
            
            _startingUnitigstest.push_back(unitig);
        }
        //std::sort(_startingUnitigstest.begin(), _startingUnitigstest.end(), UnitigComparator_ByAbundance2);
        std::sort(_startingUnitigstest.begin(), _startingUnitigstest.end(), UnitigComparator_ByLength);
        _logFile << "Nb starting unitigs test: " << _startingUnitigstest.size() << endl;

        auto rng = std::default_random_engine {};
		unordered_set<u_int32_t> binNodes;
		
		
		//unordered_set<u_int32_t> visitedNodes;
		vector<u_int32_t> startingNodesIndex;

		//vector<u_int32_t> unitigLength_cutoffs = {100000, 50000, 30000, 10000, 5000};
		vector<u_int32_t> unitigLength_cutoffs = {10000};

		_startingUnitigs.resize(unitigLength_cutoffs.size());

		unordered_set<u_int32_t> usedNodeNames;
		for(size_t i=0; i<unitigLength_cutoffs.size(); i++){
				
			u_int32_t unitigLength_cutoff_min = unitigLength_cutoffs[i];
			u_int32_t unitigLength_cutoff_max = -1;
			if(i > 0){
				unitigLength_cutoff_max = unitigLength_cutoffs[i-1];
			}

			vector<UnitigLength>& unitigLengths = _startingUnitigs[i];

            for(const Unitig& unitig: _unitigs){
				//_logFile << unitig._length << " " << unitig._abundance << endl;
				//if(unitig._index % 2 == 1) continue;
				if(unitig._length < unitigLength_cutoff_min) continue;
				if(unitig._length > unitigLength_cutoff_max) continue;
				//if(unitig._abundance < 10) continue; //200


				vector<u_int32_t> nodes;
				//Unitig& unitig = graphSimplify->_unitigs[unitig._index];

				//unitigLength._abundance = unitig._abundance;

				//float abundanceCutoff_min = computeAbundanceCutoff_min(unitigLength._abundance);
				//if(unitigLength._abundance < 100) continue;


				
				getUnitigNodes(unitig, nodes);

				u_int32_t startNodeIndex = nodes[nodes.size()/2];
                u_int64_t ray = 1;
                while(true){

                    u_int32_t index1 = nodes.size()/2 - ray;
                    u_int32_t index2 = nodes.size()/2 + ray;
                    if(index1 < 0 || index2 >= nodes.size()) break;

                    if(!_isBubble[nodes[index1]]){
                        startNodeIndex = nodes[index1];
                        break;
                    }
                    else if(!_isBubble[nodes[index2]]){
                        startNodeIndex = nodes[index2];
                        break;
                    }
                    //_logFile << BiGraph::nodeIndex_to_nodeName(nodes[index1]) << " " << BiGraph::nodeIndex_to_nodeName(nodes[index2]) << " " << " " << _isBubble[nodes[index1]] << " " << _isBubble[nodes[index2]] << " " << ray << endl;
                    ray += 1;
                }

                //getchar();
				/*
				_logFile << endl << "----" << endl;
				for(u_int32_t nIndex : nodes){
					u_int32_t nName = BiGraph::nodeIndex_to_nodeName(nIndex);
					_logFile << nName << " " << graphSimplify->_nodeAbundances[nName] << endl;
				}*/




				//std::shuffle(nodes.begin(), nodes.end(), rng);
				/*
				bool isValid = true;
				u_int32_t maxAbundance = 0;
				u_int32_t startNodeIndex = -1;
				for(u_int32_t nodeIndex : nodes){

					u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(nodeIndex);

					if(usedNodeNames.find(nodeName) != usedNodeNames.end()){
						isValid = false;
						break;
					}


					usedNodeNames.insert(nodeName);
					//if(graphSimplify->_isBubble[nodeIndex]) continue;

					u_int32_t abundance = graphSimplify->_nodeAbundances[nodeName];
					if(abundance > maxAbundance){
						maxAbundance = abundance;
						startNodeIndex = nodeIndex;
					}
					//startNodeIndex = nodeIndex;
					//unitigLength._startNodeIndex = nodeIndex;
					//break;
				}

				if(!isValid) continue;
				*/

				/*
				if(BiGraph::nodeIndex_to_nodeName(startNodeIndex) == 2246258){
					_logFile << "-" << endl;
					_logFile << "OMG" << endl;
					_logFile << unitig._abundance << endl;

					for(u_int32_t nIndex : nodes){
						u_int32_t nName = BiGraph::nodeIndex_to_nodeName(nIndex);
						_logFile << nName << " " << graphSimplify->_nodeAbundances[nName] << endl;
					}
					exit(1);
				}
				*/

				unitigLengths.push_back({unitig._length, unitig._abundance, startNodeIndex});
			}

			std::sort(unitigLengths.begin(), unitigLengths.end(), UnitigComparator_ByAbundance);


			_logFile << "Length cutoff: " << unitigLength_cutoff_min << " " << unitigLength_cutoff_max << endl;
			_logFile << "Nb starting unitigs: " << unitigLengths.size() << endl;
		}
		usedNodeNames.clear();

    }

    u_int64_t removeErrors_2(size_t k, float abundanceCutoff_min, float& currentCutoff, SaveState2& saveState, const vector<UnitigData>& unitigDatas, bool detectRoundabout, u_int64_t maxBubbleLength, bool insertBubble, bool saveAllState, bool doesSaveUnitigGraph, MDBG* mdbg, size_t minimizerSize, size_t nbCores){

        //return 0;
        
        //unordered_set<u_int32_t> 
        u_int64_t nbErrorsRemoved = 0;
        float localCutoffRate = 0.5;
        float cutoffGlobal = 1;
        float aplha = 0.1;
        float t=1.1;
        currentCutoff = min(t, abundanceCutoff_min);
        u_int32_t prevCutoff = -1;
        while(t < abundanceCutoff_min){ 
            
            if(saveAllState) checkSaveState(currentCutoff, unitigDatas, detectRoundabout, maxBubbleLength, saveState, insertBubble, doesSaveUnitigGraph, mdbg, minimizerSize, nbCores);

            currentCutoff = t;
            unordered_set<u_int32_t> removedUnitigs;

            #ifdef PRINT_DEBUG_SIMPLIFICATION
                _logFile << t << " " << abundanceCutoff_min << endl;
            #endif

            _logFile << t << " " << abundanceCutoff_min << endl;

            compact(true, unitigDatas);

            
            for(size_t i=0; i<_unitigs.size(); i+=2){
                const Unitig& unitig = _unitigs[i];
                if(unitig._startNode == -1) continue;
                //if(unitig._index % 2 == 1) continue;
                //_logFile << unitig._nbNodes << " " << unitig._abundance << endl;
                /*
                if(unitig._nbNodes > 3*k) continue;
                */
               /*
                if(unitig._nbNodes < 3*k) continue;
                
                    bool isValid = true;
                    vector<u_int32_t> successors;
                    getSuccessors_unitig(unitig._index, 0, successors);

                    for(u_int32_t unitigIndex_succ : successors){
                        vector<u_int32_t> predecessors;
                        getPredecessors_unitig(unitigIndex_succ, 0, predecessors);
                        if(predecessors.size() <= 1){
                            isValid = false;
                            break;
                        }
                    }

                    vector<u_int32_t> predecessors;
                    getPredecessors_unitig(unitig._index, 0, predecessors);

                    for(u_int32_t unitigIndex_pred : predecessors){
                        vector<u_int32_t> successors;
                        getSuccessors_unitig(unitigIndex_pred, 0, successors);
                        if(successors.size() <= 1){
                            isValid = false;
                            break;
                        }
                    }

                    if(!isValid) continue;
                }
                */
                //double localabundance = computeLocalAbundance(unitig._index, k, nodeLongNeighbors);
                double cutoff = t; //min(t, localabundance*0.5);

                if(unitig._abundance < cutoff){
                    //removedUnitigs.push_back(unitig._index);
                    removedUnitigs.insert(unitig._index);
                    removedUnitigs.insert(unitig._index+1);
                    
                    vector<u_int32_t> unitigNodes;
                    getUnitigNodes(unitig, unitigNodes);
                    for(u_int32_t node : unitigNodes){
                        //removedNodes.insert(node);
                        //removedNodes.insert(nodeIndex_toReverseDirection(node));
                        saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(node));

                        nbErrorsRemoved += 1;
                    }
                    
                }


            }
            
            //for(u_int32_t nodeIndex : removedNodes){
            //    removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
            //}
            removeUnitigs(removedUnitigs);
            for(u_int32_t unitigIndex : removedUnitigs){
                for(u_int32_t nodeIndex : _unitigs[unitigIndex]._nodes){
                    _isNodeRemoved[nodeIndex] = true;
                    //_isNodeValid2.erase(nodeIndex);
                    _abcutoff_checksum += nodeIndex;
                    //file_debug << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "red" << endl;
                    //_removedFrom[nodeIndex] = 4;
                }
            }
            

            t = t * (1+aplha);

            //_logFile << "Nb error removed: " << nbErrorsRemoved << endl;
            if(nbErrorsRemoved > 0) break;
        }

        return nbErrorsRemoved;

    }

    void removeErrors_4(size_t k, const vector<UnitigData>& unitigDatas){

        //return;
        u_int64_t nbRemoved = 0;
        bool isErrorRemoved = true;

        while(true){ 
            
            isErrorRemoved = false;
            
            unordered_set<u_int32_t> removedNodes;

            compact(true, unitigDatas);

            

            unordered_set<u_int32_t> writtenUnitigs;


            vector<UnitigTip> unitigTips;
            for(const Unitig& u: _unitigs){
                if(u._startNode == -1) continue;
                if(u._nbNodes != 1) continue;

                if(u._abundance > 1) continue;

                /*
                vector<u_int32_t> successors;
                getSuccessors_unitig(u._index, 0, successors);
                if(successors.size() != 0) continue;

                vector<u_int32_t> predecessors;
                getPredecessors_unitig(u._index, 0, predecessors);
                if(predecessors.size() != 0) continue;
                */

                //if(u._startNode % 2 != 0) continue;

                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._startNode)) != writtenUnitigs.end()) continue;
                //if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._endNode)) != writtenUnitigs.end()) continue;

                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._startNode));
                //writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._endNode));
                
                unitigTips.push_back({u._index, u._startNode, u._length});
            }

            //std::sort(unitigTips.begin(), unitigTips.end(), UnitigTipComparator_ByLength_Reverse);



            for(const UnitigTip& unitigTip : unitigTips){

                const Unitig& unitig = _unitigs[unitigTip._unitigIndex];

                if(unitig._abundance <= 1){ //unitig._nbNodes < k*2 && 
                    isErrorRemoved = true;
                    vector<u_int32_t> unitigNodes;
                    getUnitigNodes(unitig, unitigNodes);
                    for(u_int32_t node : unitigNodes){
                        removedNodes.insert(node);
                        removedNodes.insert(nodeIndex_toReverseDirection(node));
                        //saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(node));
                    }
                    nbRemoved += 1;
                }


            }
            
            unordered_set<u_int32_t> removedUnitigs;
            for(u_int32_t nodeIndex : removedNodes){
                removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
            }
            removeUnitigs(removedUnitigs);
            for(u_int32_t nodeIndex : removedNodes){
                _isNodeRemoved[nodeIndex] = true;
                //_isNodeValid2.erase(nodeIndex);
                //file_debug << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "red" << endl;
                //_removedFrom[nodeIndex] = 4;
            }
            
            if(!isErrorRemoved) break;

        }

        _logFile << "Nb errors removed: " << nbRemoved << endl;
        compact(false, unitigDatas);
    }

    u_int64_t removeErrors_3(size_t k, float abundanceCutoff_min, const vector<UnitigData>& unitigDatas, bool detectRoundabout, u_int64_t maxBubbleLength, bool insertBubble, bool saveAllState, bool doesSaveUnitigGraph, MDBG* mdbg, size_t minimizerSize, size_t nbCores){

        //return 0;
        


        unordered_set<u_int32_t> safeUnitigs;
        u_int64_t nbErrorsRemoved = 0;
        float localCutoffRate = 0.5;
        float cutoffGlobal = 1;
        float aplha = 0.1;
        float t=1;
        //currentCutoff = min(t, abundanceCutoff_min);
        u_int32_t prevCutoff = -1;



        while(t < abundanceCutoff_min){ 
            
            //if(saveAllState) checkSaveState(currentCutoff, unitigDatas, detectRoundabout, maxBubbleLength, saveState, insertBubble, doesSaveUnitigGraph, mdbg, minimizerSize, nbCores);

            //currentCutoff = t;
            unordered_set<u_int32_t> removedUnitigs;
            unordered_set<u_int32_t> removedNodes;

            #ifdef PRINT_DEBUG_SIMPLIFICATION
                _logFile << t << " " << abundanceCutoff_min << endl;
            #endif

            _logFile << t << " " << abundanceCutoff_min << endl;

            compact(true, unitigDatas);

            auto start = high_resolution_clock::now();
            
            for(const Unitig& unitig: _unitigs){
                if(unitig._startNode == -1) continue;
                if(unitig._nbNodes > _kminmerSize) continue;
                if(unitig._abundance > t) continue;
                if(safeUnitigs.find(unitig._index) != safeUnitigs.end()) continue;
                //_logFile << unitig._nbNodes << " " << unitig._abundance << endl;
                /*
                if(unitig._nbNodes > 3*k) continue;
                */
               /*
                if(unitig._nbNodes < 3*k) continue;
                
                    bool isValid = true;
                    vector<u_int32_t> successors;
                    getSuccessors_unitig(unitig._index, 0, successors);

                    for(u_int32_t unitigIndex_succ : successors){
                        vector<u_int32_t> predecessors;
                        getPredecessors_unitig(unitigIndex_succ, 0, predecessors);
                        if(predecessors.size() <= 1){
                            isValid = false;
                            break;
                        }
                    }

                    vector<u_int32_t> predecessors;
                    getPredecessors_unitig(unitig._index, 0, predecessors);

                    for(u_int32_t unitigIndex_pred : predecessors){
                        vector<u_int32_t> successors;
                        getSuccessors_unitig(unitigIndex_pred, 0, successors);
                        if(successors.size() <= 1){
                            isValid = false;
                            break;
                        }
                    }

                    if(!isValid) continue;
                }
                */
                //double localabundance = computeLocalAbundance(unitig._index, k, nodeLongNeighbors);
                double abundanceSum = 0;
                double abundanceN = 0;


                vector<u_int32_t> successors;
                getSuccessors_unitig(unitig._index, 0, successors);
                vector<u_int32_t> predecessors;
                getPredecessors_unitig(unitig._index, 0, predecessors);


                for(u_int32_t unitigIndex : successors){
                    //if(_unitis[unitigIndex]._ngbNodes < _kminmerSize) continue;
                    abundanceSum += _unitigs[unitigIndex]._abundance * _unitigs[unitigIndex]._nbNodes;
                    abundanceN += _unitigs[unitigIndex]._nbNodes;
                    //if(_graph->_unitigs[unitigIndex]._abundance < minAbundance){
                    //	minAbundance = _graph->_unitigs[unitigIndex]._abundance;
                    //}
                }
                for(u_int32_t unitigIndex : predecessors){
                    //if(_unitigs[unitigIndex]._nbNodes < _kminmerSize) continue;
                    abundanceSum += _unitigs[unitigIndex]._abundance * _unitigs[unitigIndex]._nbNodes;
                    abundanceN += _unitigs[unitigIndex]._nbNodes;
                    //if(_graph->_unitigs[unitigIndex]._abundance < minAbundance){
                    //	minAbundance = _graph->_unitigs[unitigIndex]._abundance;
                    //}
                }

                if(abundanceN == 0) continue;
                float mean = abundanceSum / abundanceN;

                //if(BiGraph::nodeIndex_to_nodeName(unitig._startNode) == 13568 || BiGraph::nodeIndex_to_nodeName(unitig._endNode) == 13568){
                //    _logFile << "loulou: " <<  unitig._abundance << " " << mean << endl;
                //}
                //if(unitig._abundance < mean*0.25){
                //    continue;
                //}
                //if(u._abundance < minAbundance){
                //	continue;
                //}


                float cutoff = min(t, mean*0.1f);
                //double cutoff = mean*0.1;

                if(unitig._abundance <= cutoff){
                    
                    //for(u_int32_t nodeIndex : unitig._nodes){
                        //removedNodes.insert(node);
                        //removedNodes.insert(nodeIndex_toReverseDirection(node));
                        removedUnitigs.insert(unitig._index);
                        //saveState._nodeNameRemoved_tmp.insert(BiGraph::nodeIndex_to_nodeName(node));

                        nbErrorsRemoved += 1;
                        //break;
                    //}

                }
                else{
                    safeUnitigs.insert(unitig._index);
                }


            }
            
            //auto stop = high_resolution_clock::now();
            //_logFile << "Duration: " << duration_cast<seconds>(stop - start).count() << endl;


            //unordered_set<u_int32_t> removedUnitigs;
            //for(u_int32_t nodeIndex : removedNodes){
            //    removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
            //}


            //auto start1 = high_resolution_clock::now();
            removeUnitigs(removedUnitigs);
            
            for(u_int32_t unitigIndex : removedUnitigs){
                for(u_int32_t nodeIndex : _unitigs[unitigIndex]._nodes){
                    _isNodeRemoved[nodeIndex] = true;
                    _isNodeRemoved[nodeIndex_toReverseDirection(nodeIndex)] = true;
                }
            }
            
            for(u_int32_t unitigIndex : _removedUnitigs){
                if(safeUnitigs.find(unitigIndex) != safeUnitigs.end()){
                    safeUnitigs.erase(unitigIndex);
                }
            }

            t = t * (1+aplha);
            

            //auto stop1 = high_resolution_clock::now();
            //_logFile << "Duration: " << duration_cast<seconds>(stop1 - start1).count() << endl;
            //exit(1);
            //_logFile << nbErrorsRemoved << endl;
            //if(nbErrorsRemoved > 0) break;
        }

        //_logFile << "Removed nodes: " << nbErrorsRemoved << endl;
        return nbErrorsRemoved;

    }

    /*
    u_int64_t removeErrors(size_t k, unordered_map<u_int32_t, vector<LongNeighbor>>& nodeLongNeighbors, const vector<UnitigData>& unitigDatas){


        //unordered_set<u_int32_t> 
        u_int64_t nbErrorsRemoved = 0;
        float localCutoffRate = 0.5;
        float cutoffGlobal = 1;
        float aplha = 0.1;
        double t=2.5;
        u_int32_t prevCutoff = -1;
        while(t < 500){ //500 = max unitig abundance over graph
            
            unordered_set<u_int32_t> removedNodes;
            //if(((u_int32_t)t) != prevCutoff){
            //    prevCutoff = t;

            _logFile << t << endl;
            //_logFile << "Nb node valid: " << _isNodeValid2.size() << endl;

            compact(true, unitigDatas);
                
            //t = t * (1+aplha);
            //continue;
                


            
            for(const Unitig& unitig: _unitigs){
                if(unitig._startNode == -1) continue;
                //_logFile << unitig._nbNodes << " " << unitig._abundance << endl;
                if(unitig._nbNodes > 3*k) continue;

                
                bool isValid = true;
                vector<u_int32_t> successors;
                getSuccessors_unitig(unitig._index, 0, successors);

                for(u_int32_t unitigIndex_succ : successors){
                    vector<u_int32_t> predecessors;
                    getPredecessors_unitig(unitigIndex_succ, 0, predecessors);
                    if(predecessors.size() <= 1){
                        isValid = false;
                        break;
                    }
                }

                vector<u_int32_t> predecessors;
                getPredecessors_unitig(unitig._index, 0, predecessors);

                for(u_int32_t unitigIndex_pred : predecessors){
                    vector<u_int32_t> successors;
                    getSuccessors_unitig(unitigIndex_pred, 0, successors);
                    if(successors.size() <= 1){
                        isValid = false;
                        break;
                    }
                }

                if(!isValid) continue;
                

                //if(unitig._nbNodes > 3*k) continue;

                //_logFile << "mioum: " << _unitigs[0]._startNode << endl;
                //_logFile << unitig._index << " " << _unitigs.size() << endl;


                //_logFile << "1" << endl;
                double localabundance = computeLocalAbundance(unitig._index, k, nodeLongNeighbors);
                double cutoff = min(t, localabundance*0.5);

                //_logFile << "2" << endl;
                if(unitig._abundance < cutoff){
                    vector<u_int32_t> unitigNodes;
                    getUnitigNodes(unitig, unitigNodes);
                    for(u_int32_t node : unitigNodes){
                        removedNodes.insert(node);
                        removedNodes.insert(nodeIndex_toReverseDirection(node));

              

                        //if(BiGraph::nodeIndex_to_nodeName(node) == 314212){
                          //  _logFile << BiGraph::nodeIndex_to_nodeName(node) << " " << unitig._abundance << " " << localabundance << endl;
                            //getchar();
                        //}
                        

                        //bool isContigNode = _unitigDatas[BiGraph::nodeIndex_to_nodeName(node)]._readIndexes.size() == 0;
                        //if(isContigNode){
                        //    _logFile << "Removing contig node: " << localabundance << endl;
                        //    getchar();
                        //}

                        //_isNodeValid2.erase(node);
                        nbErrorsRemoved += 1;
                    }
                    //isModification = true;
                }
                //_logFile << "3" << endl;
                //_logFile << BiGraph::nodeIndex_to_nodeName(unitig._startNode) << " " << unitig._abundance << " " << computeLocalAbundance(unitig._index, k) << endl;
                //if(localabundance < 30) getchar();
                //_logFile << BiGraph::nodeIndex_to_nodeName(unitig._startNode) << " " << unitig._abundance << " " << computeLocalAbundance(unitig._index, k) << endl;
                //if(computeLocalAbundance(unitig._index, k) > 50){
                //    getchar();
                //}


                //if(20545 == BiGraph::nodeIndex_to_nodeName(unitig._startNode) || 20545 == BiGraph::nodeIndex_to_nodeName(unitig._endNode)){
                //    _logFile << unitig._abundance << " " << localabundance << " " << cutoff << endl;
                //    getchar();
                //}

            }
            //}
            
            unordered_set<u_int32_t> removedUnitigs;
            for(u_int32_t nodeIndex : removedNodes){
                //if(_nodeToUnitig.find(nodeIndex) == _nodeToUnitig.end()){
                //    _logFile << "KOUERK" << endl;
                //}
                //_unitigs[_nodeToUnitig[nodeIndex]]._index



                removedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
            }
            removeUnitigs(removedUnitigs);
            for(u_int32_t nodeIndex : removedNodes){
                _isNodeRemoved[nodeIndex] = true;
                //_isNodeValid2.erase(nodeIndex);
                //file_debug << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "red" << endl;
                //_removedFrom[nodeIndex] = 4;
            }
            

            t = t * (1+aplha);

            //if(nbErrorsRemoved > 0) break;
        }

        return nbErrorsRemoved;
    }
    */
    /*
    void saveCurrentGfa(u_int32_t nodeIndex_source, u_int32_t abundance, u_int32_t binIndex){



        _logFile << "Saving current gfa" << endl;
        //Bizarre mais peut etre detruit dans les region pourri composé de 3 unitig relié a leur extreminité (donc 3 tips) par exemple
        if(_isNodeValid2.find(nodeIndex_source) == _isNodeValid2.end()){
            return;
        }


        unordered_set<u_int32_t> memoNodes = _isNodeValid2;
        for(u_int32_t nodeIndex : memoNodes){
            if(getNodeUnitigAbundance(nodeIndex) < abundance/4){
                _isNodeValid2.erase(nodeIndex);
            }
        }

        unordered_set<u_int32_t> component;
        getConnectedComponent(nodeIndex_source, component);

        _isNodeValid2 = memoNodes;


        unordered_set<u_int32_t> isNodeValid2;

        //_logFile << component.size() << endl;
        //u_int32_t componentSize = 0;

        vector<u_int32_t> nodes;
        for(u_int32_t unitigIndex : component){
            getUnitigNodes(_unitigs[unitigIndex], nodes);
            for(u_int32_t nodeIndex : nodes){
                isNodeValid2.insert(nodeIndex);
                //_isNodeValid[nodeIndex] = true;
                //componentSize += 1;
            }
        }


        fs::path path(_outputDir + "/gfa");
	    if(!fs::exists (path)){
            fs::create_directory(path);
        } 
        //System::file().mkdir(_outputDir + "/gfa", -1);

        //fs::path p(_outputDir + "/gfa");
        //if(!fs::exists(p)){
        //    fs::create_directory(_outputDir + "/gfa");
        //}

        _logFile << "Component size: " << isNodeValid2.size() << endl;
        
        unordered_set<u_int32_t> validNodes;
        for (auto& nodeIndex : isNodeValid2){
            u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
            validNodes.insert(nodeName);
        }
        string outputFilename = _outputDir + "/gfa/" + to_string(binIndex) + "_" + to_string(abundance) + "_" + to_string(BiGraph::nodeIndex_to_nodeName(nodeIndex_source)) + ".gfa";
        GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, outputFilename, validNodes, _isEdgeRemoved, _graphSuccessors);
        //getchar();

    }
    */
    
    void saveGraph(const string& outputFilename){

        unordered_set<u_int32_t> validNodes;
        for(size_t nodeIndex=0; nodeIndex<_graphSuccessors->_nbNodes; nodeIndex++){
            if(_isNodeRemoved[nodeIndex]) continue;
            u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
            //if(_debug_groundTruthNodeNames.find(nodeName) == _debug_groundTruthNodeNames.end()) continue;
            validNodes.insert(nodeName);
        }
        
        GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, outputFilename, validNodes, _graphSuccessors);
    }

    void saveGraph_aroundNode(const string& outputFilename, u_int32_t nodeIndex_source){

        unordered_set<u_int32_t> neighbors;
        collectNeighbors(nodeIndex_source, 20, 10000, neighbors);

        unordered_set<u_int32_t> validNodes;
        for (auto& nodeIndex : neighbors){
            u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
            //if(_debug_groundTruthNodeNames.find(nodeName) == _debug_groundTruthNodeNames.end()) continue;
            validNodes.insert(nodeName);
        }
        
        GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, outputFilename, validNodes, _graphSuccessors);
    }


    void saveGraph2(const string& outputFilename, unordered_set<u_int32_t>& componentOnly){

        _logFile << "Saving graph: " << outputFilename << endl;

	    ofstream outputFile(outputFilename);

        unordered_set<u_int32_t> nodeNames;

        vector<u_int32_t> successors;
        for(size_t nodeIndex=0; nodeIndex<_graphSuccessors->_nbNodes; nodeIndex++){
            if(_isNodeRemoved[nodeIndex]) continue;

            u_int32_t unitigIndex = nodeIndex_to_unitigIndex(nodeIndex);
            if(componentOnly.find(unitigIndex) == componentOnly.end()) continue;

            bool ori;
            u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(nodeIndex, ori); 

            string ori_str = "+";
            if(!ori) ori_str = "-";

            nodeNames.insert(nodeName);

            getSuccessors(nodeIndex, 0, successors);
            
            for(u_int32_t nodeIndex_succ : successors){
                bool ori2;
                u_int32_t nodeName2 = BiGraph::nodeIndex_to_nodeName(nodeIndex_succ, ori2); 

                string ori_str2 = "+";
                if(!ori2) ori_str2 = "-";

                outputFile << "L" << "\t" << nodeName << "\t" << ori_str << "\t" << nodeName2 << "\t" << ori_str2 << "\t" <<  _kminmerOverlapMean << "M" << endl;
            }
            //L	41056	+	51634	+	481M
        }

        for(u_int32_t nodeName : nodeNames){


            outputFile << "S" << "\t" << nodeName << "\t" << "*" << "\t" << "LN:i:" << _graphSuccessors->_nodeDatas[nodeName]._length << "\t" << "dp:i:" << _graphSuccessors->_nodeDatas[nodeName]._abundance << endl;
            //S	41064	*	LN:i:676	dp:i:33
        }

        outputFile.close();

        _logFile << "done" << endl;
        
    }

    void getConnectedComponent(u_int32_t nodeIndex_source, unordered_set<u_int32_t>& component){

        component.clear();

        vector<u_int32_t> neighbors;
        //unordered_set<u_int32_t> isVisited;

        //for(size_t n=0; n<_nbNodes; n++){
        //    if(isVisited[n]) continue;

        queue <u_int32_t> queue;

        //_logFile << "----------" << endl;
        //_logFile << nodeIndex_source << endl;
        //_logFile << _unitigs.size() << endl;
        //_logFile << _nodeToUnitig[nodeIndex_source] << endl;


        queue.push(nodeIndex_to_unitigIndex(nodeIndex_source));
        component.insert(nodeIndex_to_unitigIndex(nodeIndex_source));
        //component.push_back(nodeIndex_source);

        while (!queue.empty()){

            u_int64_t unitigIndex = queue.front();
            //_logFile << "1: " << unitigIndex << endl;

            queue.pop();

            vector<u_int32_t> neighbors1;
            getNeighbors(_unitigs[unitigIndex]._startNode, 0, neighbors1);
            vector<u_int32_t> neighbors2;
            getNeighbors(_unitigs[unitigIndex]._endNode, 0, neighbors2);


            for(u_int32_t nodeIndex_neighbor : neighbors1){
                //_logFile << "2: " << _nodeToUnitig[nodeIndex_neighbor] << endl;
                u_int32_t unitigIndex_neighbor = nodeIndex_to_unitigIndex(nodeIndex_neighbor);
                if (component.find(unitigIndex_neighbor) != component.end()) continue;

                queue.push(unitigIndex_neighbor);

                component.insert(unitigIndex_neighbor);
                //component.push_back(nodeIndex_neighbor);
            }

            for(u_int32_t nodeIndex_neighbor : neighbors2){
                //_logFile << "3: " << _nodeToUnitig[nodeIndex_neighbor] << endl;
                u_int32_t unitigIndex_neighbor = nodeIndex_to_unitigIndex(nodeIndex_neighbor);
                if (component.find(unitigIndex_neighbor) != component.end()) continue;

                queue.push(unitigIndex_neighbor);

                component.insert(unitigIndex_neighbor);
                //component.push_back(nodeIndex_neighbor);
            }
        }

    }

    void getConnectedComponent_unitig(u_int32_t unitigIndex_source, unordered_set<u_int32_t>& component){

        //_logFile << "-----" << endl;
        //_logFile << unitigIndex_source << endl;
        component.clear();

        queue <u_int32_t> queue;

        queue.push(unitigIndex_source);

        while (!queue.empty()){

            u_int64_t unitigIndex = queue.front();
            queue.pop();

            //_logFile << unitigIndex << endl;
            if(_unitigs[unitigIndex]._startNode == -1 || _unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode == -1){
                _logFile << "lala" << endl;
                _logFile << _unitigs[unitigIndex]._index << " " << _unitigs[unitigIndex_toReverseDirection(unitigIndex)]._index << endl;
                _logFile << _unitigs[unitigIndex]._startNode << " " << _unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode << endl;
                getchar();
            }
            if(_unitigs[unitigIndex]._startNode == -1) continue;
            if(_unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode == -1) continue;
            //if(_unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode == -1) continue;


            if (component.find(unitigIndex) != component.end()) continue;

            //_logFile << "Start: " << unitigIndex << " " << unitigIndex_toReverseDirection(unitigIndex) << endl;
            //_logFile << _unitigs[unitigIndex]._startNode << " " << _unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode << endl;
            //_logFile << "1" << endl;

            component.insert(unitigIndex);
            component.insert(unitigIndex_toReverseDirection(unitigIndex));


            //_logFile << "2" << endl;
            vector<u_int32_t> successors;

            getSuccessors_unitig(unitigIndex, 0, successors);
            for(u_int32_t unitigIndex_nn : successors) queue.push(unitigIndex_nn);

            //_logFile << "3" << endl;
            getPredecessors_unitig(unitigIndex, 0, successors);
            for(u_int32_t unitigIndex_nn : successors) queue.push(unitigIndex_nn);

            //_logFile << "4" << endl;
            getSuccessors_unitig(unitigIndex_toReverseDirection(unitigIndex), 0, successors);
            for(u_int32_t unitigIndex_nn : successors) queue.push(unitigIndex_nn);

            //_logFile << "5" << endl;
            getPredecessors_unitig(unitigIndex_toReverseDirection(unitigIndex), 0, successors);
            for(u_int32_t unitigIndex_nn : successors) queue.push(unitigIndex_nn);

            //_logFile << "6" << endl;
        }

    }


    void getConnectedComponent_unitig(u_int32_t unitigIndex_source, u_int32_t maxDistance, unordered_set<u_int32_t>& component){

        //_logFile << "-----" << endl;
        //_logFile << unitigIndex_source << endl;
        component.clear();

        queue <u_int32_t> queue;

        queue.push(unitigIndex_source);
        queue.push(unitigIndex_toReverseDirection(unitigIndex_source));
        unordered_map<u_int32_t, u_int32_t> distances;

        distances[unitigIndex_source] = 0;
        distances[unitigIndex_toReverseDirection(unitigIndex_source)] = 0;

        while (!queue.empty()){

            u_int64_t unitigIndex = queue.front();
            queue.pop();

            //_logFile << unitigIndex << endl;
            if(_unitigs[unitigIndex]._startNode == -1 || _unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode == -1){
                _logFile << "lala" << endl;
                _logFile << _unitigs[unitigIndex]._index << " " << _unitigs[unitigIndex_toReverseDirection(unitigIndex)]._index << endl;
                _logFile << _unitigs[unitigIndex]._startNode << " " << _unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode << endl;
                getchar();
            }
            if(_unitigs[unitigIndex]._startNode == -1) continue;
            if(_unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode == -1) continue;
            //if(_unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode == -1) continue;


            if (component.find(unitigIndex) != component.end()) continue;

            if(distances[unitigIndex] > maxDistance) continue;

            //_logFile << "Start: " << unitigIndex << " " << unitigIndex_toReverseDirection(unitigIndex) << endl;
            //_logFile << _unitigs[unitigIndex]._startNode << " " << _unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode << endl;
            //_logFile << "1" << endl;

            component.insert(unitigIndex);
            component.insert(unitigIndex_toReverseDirection(unitigIndex));


            //_logFile << "2" << endl;
            vector<u_int32_t> neighbors;
            vector<u_int32_t> successors;

            getSuccessors_unitig(unitigIndex, 0, successors);
            for(u_int32_t unitigIndex_nn : successors) neighbors.push_back(unitigIndex_nn);

            //_logFile << "3" << endl;
            getPredecessors_unitig(unitigIndex, 0, successors);
            for(u_int32_t unitigIndex_nn : successors) neighbors.push_back(unitigIndex_nn);

            //_logFile << "4" << endl;
            getSuccessors_unitig(unitigIndex_toReverseDirection(unitigIndex), 0, successors);
            for(u_int32_t unitigIndex_nn : successors) neighbors.push_back(unitigIndex_nn);

            //_logFile << "5" << endl;
            getPredecessors_unitig(unitigIndex_toReverseDirection(unitigIndex), 0, successors);
            for(u_int32_t unitigIndex_nn : successors) neighbors.push_back(unitigIndex_nn);

            for(u_int32_t unitigIndex_nn : neighbors){

                distances[unitigIndex_nn] = distances[unitigIndex] + 1;


                queue.push(unitigIndex_nn);
            }

        }

    }

    void getConnectedComponent_unitig_nt(u_int32_t unitigIndex_source, u_int32_t maxDistance, unordered_set<u_int32_t>& component){

        //_logFile << "-----" << endl;
        //_logFile << unitigIndex_source << endl;
        component.clear();

        queue <u_int32_t> queue;

        queue.push(unitigIndex_source);
        queue.push(unitigIndex_toReverseDirection(unitigIndex_source));
        unordered_map<u_int32_t, u_int32_t> distances;

        distances[unitigIndex_source] = 0;
        distances[unitigIndex_toReverseDirection(unitigIndex_source)] = 0;

        unordered_set<u_int32_t> isVisited;

        while (!queue.empty()){

            u_int64_t unitigIndex = queue.front();
            queue.pop();


            component.insert(unitigIndex);
            component.insert(unitigIndex_toReverseDirection(unitigIndex));

            if (isVisited.find(unitigIndex) != isVisited.end()) continue;

            isVisited.insert(unitigIndex);
            isVisited.insert(unitigIndex_toReverseDirection(unitigIndex));

            if(distances[unitigIndex] > maxDistance) continue;

            //_logFile << "Start: " << unitigIndex << " " << unitigIndex_toReverseDirection(unitigIndex) << endl;
            //_logFile << _unitigs[unitigIndex]._startNode << " " << _unitigs[unitigIndex_toReverseDirection(unitigIndex)]._startNode << endl;
            //_logFile << "1" << endl;



            //_logFile << "2" << endl;
            vector<AdjNode> neighbors;
            vector<AdjNode> successors;

            getSuccessors_unitig_overlap(unitigIndex, successors);
            for(const AdjNode& unitigIndex_nn : successors) neighbors.push_back(unitigIndex_nn);

            //_logFile << "3" << endl;
            getPredecessors_unitig_overlap(unitigIndex, successors);
            for(const AdjNode& unitigIndex_nn : successors) neighbors.push_back(unitigIndex_nn);

            //_logFile << "4" << endl;
            getSuccessors_unitig_overlap(unitigIndex_toReverseDirection(unitigIndex), successors);
            for(const AdjNode& unitigIndex_nn : successors) neighbors.push_back(unitigIndex_nn);

            //_logFile << "5" << endl;
            getPredecessors_unitig_overlap(unitigIndex_toReverseDirection(unitigIndex), successors);
            for(const AdjNode& unitigIndex_nn : successors) neighbors.push_back(unitigIndex_nn);


            for(const AdjNode& unitigNode : neighbors){

                u_int32_t unitigIndex_nn = unitigNode._index;
                u_int32_t overlap = _kminmerOverlapMean;

                //_logFile << distances[unitigIndex] << " " << _unitigs[unitigIndex_nn]._length << " " << overlap << endl;
                distances[unitigIndex_nn] = distances[unitigIndex] + _unitigs[unitigIndex_nn]._length - overlap; 

                queue.push(unitigIndex_nn);
            }

        }

    }

    /*
    void removeUnconnectedNodes(u_int32_t nodeIndex_source){
        
        //Bizarre mais peut etre detruit dans les region pourri composé de 3 unitig relié a leur extreminité (donc 3 tips) par exemple
        if(_isNodeValid2.find(nodeIndex_source) == _isNodeValid2.end()){
            return;
        }

        
        #ifdef PRINT_DEBUG_SIMPLIFICATION
            _logFile << "Compute connected component" << endl;
            _logFile << _isNodeValid2.size() << endl;
        #endif
        unordered_set<u_int32_t> component;
        getConnectedComponent(nodeIndex_source, component);


        unordered_set<u_int32_t> isNodeValid2;

        //_logFile << component.size() << endl;
        //u_int32_t componentSize = 0;

        vector<u_int32_t> nodes;
        for(u_int32_t unitigIndex : component){
            getUnitigNodes(_unitigs[unitigIndex], nodes);
            for(u_int32_t nodeIndex : nodes){
                isNodeValid2.insert(nodeIndex);
                //_isNodeValid[nodeIndex] = true;
                //componentSize += 1;
            }
        }

        #ifdef PRINT_DEBUG_SIMPLIFICATION
            _logFile << "Component size: " << isNodeValid2.size() << endl;
        #endif
        _isNodeValid2 = isNodeValid2;



    }
    */
   /*
    double computeLocalAbundance(u_int32_t unitigIndex_source, u_int64_t k, unordered_map<u_int32_t, vector<LongNeighbor>>& nodeLongNeighbors){

        //_logFile << "---------------" << endl;
        //return 0;
        
        float abundanceMinIndexed = std::numeric_limits<float>::max();

        bool needIndexing = true;
        vector<u_int32_t> unitigNodes;
        getUnitigNodes(_unitigs[unitigIndex_source], unitigNodes);


        bool hasChanged = false;
        for(u_int32_t nodeIndex : unitigNodes){
            u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(nodeIndex);

            const auto& it = nodeLongNeighbors.find(nodeName);
            if(it != nodeLongNeighbors.end()){
                needIndexing = false;
                for(const LongNeighbor& ln : it->second){

                    if(_isNodeValid2.find(ln._nodeIndex) == _isNodeValid2.end()){
                        hasChanged = true;
                        break;
                    }


                    u_int32_t unitigIndex = nodeIndex_to_unitigIndex(ln._nodeIndex);
                    float abundance = ln._abundance;
                    if(abundance < abundanceMinIndexed){
                        abundanceMinIndexed = abundance;
                    }
                    //if(nodeName == 580)
                    //_logFile << "Existing: " << BiGraph::nodeIndex_to_nodeName(ln._nodeIndex) << " " << abundance << endl;

                    float currentAbundance = _unitigs[unitigIndex]._abundance;
                    if(currentAbundance != abundance){
                        hasChanged = true;
                        break;
                    }
                }
                //return 50;
            }
            if(hasChanged) break;
        }


        //_logFile << hasChanged << endl;
        if(hasChanged){
            needIndexing = true;
            for(u_int32_t nodeIndex : unitigNodes){
                u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(nodeIndex);
                if(nodeLongNeighbors.find(nodeName) != nodeLongNeighbors.end()){
                    //_logFile << "cleaned" << endl;
                    nodeLongNeighbors.erase(nodeName);
                }
            }
        }

        if(!hasChanged && !needIndexing){
            return abundanceMinIndexed;
        }
        
        //_logFile << "Indexed: " << abundanceMinIndexed << endl;

        static float maxVal = std::numeric_limits<float>::max();

        //_logFile << "recomp" << endl;
        int lala = 0;

        u_int64_t totalLongUnitigLength = 0;
        u_int64_t nbLongUnitigs = 0;
        float abundanceMin = maxVal;

        unordered_map<u_int32_t, u_int32_t> distance;
        //neighbors.clear();
        //if(_nodes[s] == nullptr) return;

        //neighbors.push_back(s);

        unordered_set<u_int32_t> isVisited;
    
        queue<u_int32_t> queue;
    
        //isVisited[s] = true;
        distance[unitigIndex_source] = 0;
        queue.push(unitigIndex_source);
        isVisited.insert(unitigIndex_source);
        //_logFile << "------" << endl;

        while(!queue.empty()){
            
            //if(nbLongUnitigs >= 4) break; // && totalLongUnitigLength > 1500) break;

            u_int32_t unitigIndex = queue.front();
            queue.pop();

            vector<u_int32_t> successors;
            getSuccessors_unitig(unitigIndex, 0, successors);

            vector<u_int32_t> predecessors;
            getPredecessors_unitig(unitigIndex, 0, predecessors);

            lala += 1;

            //_logFile << lala << " " << distance[unitigIndex] << " " << nbLongUnitigs << " " << abundanceMin << endl;
            if(nbLongUnitigs > 1) break;
            if(lala > 100) break;
            if(abundanceMin <= 2) return 2;
            //_logFile << unitigIndex << " " << nbLongUnitigs << " " << totalLongUnitigLength << endl;

            for(u_int32_t unitigIndex_nn : successors){

                if(isVisited.find(unitigIndex_nn) != isVisited.end()) continue;                    

                bool isLong = false;
                if(_unitigs[unitigIndex_nn]._nbNodes > 3*k){
                    //_logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_nn]._startNode) << endl;
                    //if(_unitigs[unitigIndex_nn]._nod > 1500){
                    nbLongUnitigs += 1;
                    totalLongUnitigLength += _graphSuccessors->_nodeLengths[BiGraph::nodeIndex_to_nodeName(unitigIndex_nn)];
                    if(_unitigs[unitigIndex_nn]._abundance < abundanceMin){
                        abundanceMin = _unitigs[unitigIndex_nn]._abundance;
                    }

                    if(needIndexing){
                        nodeLongNeighbors[BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_source]._startNode)].push_back({_unitigs[unitigIndex_nn]._startNode, _unitigs[unitigIndex_nn]._abundance});
                    }    
                    //if(BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_source]._startNode) == 580)
                    //_logFile << "Truth: " << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_nn]._startNode) << " " << _unitigs[unitigIndex_nn]._abundance << endl;
                    isLong = true;
                }

                distance[unitigIndex_nn] = distance[unitigIndex] + 1;
                isVisited.insert(unitigIndex_nn);
                if(!isLong && distance[unitigIndex_nn] < 10) queue.push(unitigIndex_nn);
            }

            for(u_int32_t unitigIndex_nn : predecessors){

                if(isVisited.find(unitigIndex_nn) != isVisited.end()) continue;

                bool isLong = false;

                if(_unitigs[unitigIndex_nn]._nbNodes > 3*k){
                    //if(_unitigs[unitigIndex_nn]._length >= 10000){
                    //_logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_nn]._startNode) << endl;
                    nbLongUnitigs += 1;
                    totalLongUnitigLength += _graphSuccessors->_nodeLengths[BiGraph::nodeIndex_to_nodeName(unitigIndex_nn)];
                    if(_unitigs[unitigIndex_nn]._abundance < abundanceMin){
                        abundanceMin = _unitigs[unitigIndex_nn]._abundance;
                    }
                    
                    if(needIndexing){
                        nodeLongNeighbors[BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_source]._startNode)].push_back({_unitigs[unitigIndex_nn]._startNode, _unitigs[unitigIndex_nn]._abundance});
                    }  
                    //if(BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_source]._startNode) == 580)
                    //_logFile << "Truth: " << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_nn]._startNode) << " " << _unitigs[unitigIndex_nn]._abundance << endl;
                    isLong = true;
                }

                distance[unitigIndex_nn] = distance[unitigIndex] + 1;
                isVisited.insert(unitigIndex_nn);
                if(!isLong && distance[unitigIndex_nn] < 10) queue.push(unitigIndex_nn);

            }


        }

        //if(!hasChanged && !needIndexing){
            //getchar();
        //}
        if(nbLongUnitigs <= 1) return 2;
        if(abundanceMin == maxVal) return 2;
        //_logFile << "Truth: " << abundanceMin << " " << hasChanged << " " << needIndexing << endl;
        //_logFile << "Nb nodes: " << lala << endl;
        //_logFile << abundanceMin << endl;
        //_logFile << abundanceMin << endl;
        //_logFile << "Nb long utg: " << nbLongUnitigs << " " << abundanceMin << endl;
        return abundanceMin;

    }
    */

    void collectNeighbors(u_int32_t nodeIndex_source, u_int32_t maxDistance, u_int32_t maxNeighbors, unordered_set<u_int32_t>& neighbors){

        unordered_map<u_int32_t, u_int32_t> distance;
        neighbors.clear();
        //if(_nodes[s] == nullptr) return;

        //neighbors.push_back(s);


    
        queue<u_int32_t> queue;
    
        //isVisited[s] = true;
        distance[nodeIndex_source] = 0;
        queue.push(nodeIndex_source);
        neighbors.insert(nodeIndex_source);
    
        while(!queue.empty()){
            
            u_int32_t n = queue.front();
            queue.pop();

            vector<u_int32_t> nodeNeighbors;
            getNeighbors(n, 0, nodeNeighbors);

            //_logFile << n << " " << nodeNeighbors.size() << endl;
            for(u_int32_t nodeIndex_neighbor : nodeNeighbors){


                if(neighbors.find(nodeIndex_neighbor) != neighbors.end()){
                    continue;
                }

                //if(visitedNodes.find(nn) != visitedNodes.end()){
                //    node = node->next;
                //    continue;
                //}

                if(maxNeighbors > 0 && neighbors.size() >= maxNeighbors){
                    break;
                }

                neighbors.insert(nodeIndex_neighbor);
                //isVisited[nn] = true;
                distance[nodeIndex_neighbor] = distance[n] + 1;
                //neighbors.push_back(nn);


                if(distance[nodeIndex_neighbor] >= maxDistance){
                    continue;
                }

                //_logFile << "Push: " << nn << " " << distance[nn] << endl;
                queue.push(nodeIndex_neighbor);

            }

            if(maxNeighbors > 0 && neighbors.size() >= maxNeighbors){
                break;
            }

        }



    }

    void extractComponent(u_int32_t nodeName){
        
        u_int32_t nodeIndex = BiGraph::nodeName_to_nodeIndex(nodeName, true);

        vector<u_int32_t> unitigNodes;
        getUnitigNodes(nodeIndex_to_unitig(nodeIndex), unitigNodes);
        for(u_int32_t nIndex : unitigNodes){
            u_int32_t nName = BiGraph::nodeIndex_to_nodeName(nIndex);
            //_logFile << nName << " " << _graphSuccessors->_nodeAbundances[nName] << endl;
        }


        //_logFile << nodeName << " " << nodeIndex_to_unitig(nodeIndex)._abundance << " " << nodeIndex_to_unitig(nodeIndex)._length << endl;
        //_logFile << nodeIndex_to_unitig(nodeIndex)._abundance << endl;
        
        unordered_set<u_int32_t> nodes;
		collectNeighbors(nodeIndex, 200, 10000, nodes);

        unordered_set<u_int32_t> validNodes;

        for (auto& nodeIndex : nodes){
            u_int32_t nodeName = _graphSuccessors->nodeIndex_to_nodeName(nodeIndex);
            validNodes.insert(nodeName);
        }

        GfaParser::rewriteGfa_withoutNodes(_inputGfaFilename, _inputGfaFilename + "_component.gfa", validNodes, _graphSuccessors);
		//exit(1);
    }
    
    /*

    void getStronglyConnectedComponent(u_int32_t unitigIndex_source, bool forward, vector<u_int32_t>& component){ //, vector<u_int32_t>& scc
        //scc.clear();
    
        component.clear();
        //vector<vector<u_int32_t>> sccs;

        unordered_map<u_int32_t, u_int32_t> preorder;
        unordered_map<u_int32_t, u_int32_t> lowlink;
        unordered_set<u_int32_t> scc_found;
        vector<u_int32_t> scc_queue;
        vector<u_int32_t> queue;

        u_int64_t i = 0;

        //u_int32_t unitigIndex_source = nodeIndex_to_unitigIndex(nodeIndex_source);

        queue.clear();
        queue.push_back(unitigIndex_source);

        while(queue.size() > 0){
            u_int32_t v = queue[queue.size()-1];

            if(preorder.find(v) == preorder.end()){
                i += 1;
                preorder[v] = i;
            }

            bool done = true;

            vector<u_int32_t> v_nbrs;
            if(forward){
                getSuccessors_unitig(v, 0, v_nbrs); //_unitigs[v]._endNode
            }
            else{
                getPredecessors_unitig(v, 0, v_nbrs); //_unitigs[v]._endNode
            }
            
            //_logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[v]._startNode) << " " << v_nbrs.size() << endl;

            for(u_int32_t w : v_nbrs){
                if(preorder.find(w) == preorder.end()){
                    queue.push_back(w);
                    done = false;
                    break;
                }
            }

            if(done){
                lowlink[v] = preorder[v];
                for(u_int32_t w : v_nbrs){
                    if(scc_found.find(w) == scc_found.end()){
                        if(preorder[w] > preorder[v]){
                            lowlink[v] = min(lowlink[v], lowlink[w]);
                        }
                        else{
                            lowlink[v] = min(lowlink[v], preorder[w]);
                        }
                    }
                }

                queue.pop_back();

                if(lowlink[v] == preorder[v]){
                    //_logFile << "----" << endl;
                    scc_found.insert(v);
                    vector<u_int32_t> scc = {v};

                    bool isSourceScc = false;

                    if(v == unitigIndex_source) isSourceScc = true;

                    while(scc_queue.size() > 0 && preorder[scc_queue[scc_queue.size()-1]] > preorder[v]){
                        u_int32_t k = scc_queue[scc_queue.size()-1];
                        scc_queue.pop_back();
                        scc_found.insert(k);
                        scc.push_back(k);

                        //_logFile << "Lala: " << unitigIndex_source << " " << k << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[k]._startNode) << endl;

                        if(k == unitigIndex_source) isSourceScc = true;
                    }

                    if(isSourceScc){
                        for(u_int32_t unitigIndex : scc){
                            component.push_back(unitigIndex);
                        }
                        //_logFile << "found" << endl;
                        //component = scc;
                        return;
                    }
                    //sccs.push_back(scc);
                }
                else{
                    scc_queue.push_back(v);
                }
            }
        }
        



    }

    void getStronglyConnectedComponents(u_int32_t unitigIndex_source, bool forward, vector<vector<u_int32_t>>& components){ //, vector<u_int32_t>& scc
        //scc.clear();
    
        components.clear();
        //vector<vector<u_int32_t>> sccs;

        unordered_map<u_int32_t, u_int32_t> preorder;
        unordered_map<u_int32_t, u_int32_t> lowlink;
        unordered_set<u_int32_t> scc_found;
        vector<u_int32_t> scc_queue;
        vector<u_int32_t> queue;

        u_int64_t i = 0;

        //u_int32_t unitigIndex_source = nodeIndex_to_unitigIndex(nodeIndex_source);

        queue.clear();
        queue.push_back(unitigIndex_source);

        while(queue.size() > 0){
            u_int32_t v = queue[queue.size()-1];

            if(preorder.find(v) == preorder.end()){
                i += 1;
                preorder[v] = i;
            }

            bool done = true;

            vector<u_int32_t> v_nbrs;
            if(forward){
                getSuccessors_unitig(v, 0, v_nbrs); //_unitigs[v]._endNode
            }
            else{
                getPredecessors_unitig(v, 0, v_nbrs); //_unitigs[v]._endNode
            }
            
            //_logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[v]._startNode) << " " << v_nbrs.size() << endl;

            for(u_int32_t w : v_nbrs){
                if(preorder.find(w) == preorder.end()){
                    queue.push_back(w);
                    done = false;
                    break;
                }
            }

            if(done){
                lowlink[v] = preorder[v];
                for(u_int32_t w : v_nbrs){
                    if(scc_found.find(w) == scc_found.end()){
                        if(preorder[w] > preorder[v]){
                            lowlink[v] = min(lowlink[v], lowlink[w]);
                        }
                        else{
                            lowlink[v] = min(lowlink[v], preorder[w]);
                        }
                    }
                }

                queue.pop_back();

                if(lowlink[v] == preorder[v]){
                    //_logFile << "----" << endl;
                    scc_found.insert(v);
                    vector<u_int32_t> scc = {v};

                    bool isSourceScc = false;

                    if(v == unitigIndex_source) isSourceScc = true;

                    while(scc_queue.size() > 0 && preorder[scc_queue[scc_queue.size()-1]] > preorder[v]){
                        u_int32_t k = scc_queue[scc_queue.size()-1];
                        scc_queue.pop_back();
                        scc_found.insert(k);
                        scc.push_back(k);

                        //_logFile << "Lala: " << unitigIndex_source << " " << k << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[k]._startNode) << endl;

                        if(k == unitigIndex_source) isSourceScc = true;
                    }

                    if(isSourceScc){
                        //_logFile << "found" << endl;
                        //component = scc;
                        //return;
                    }
                    components.push_back(scc);
                }
                else{
                    scc_queue.push_back(v);
                }
            }
        }
        
   

    }

    void getStronglyConnectedComponent_node(u_int32_t nodeIndex_source, bool forward, vector<u_int32_t>& component){ //, vector<u_int32_t>& scc
        //scc.clear();
    
        component.clear();
        //vector<vector<u_int32_t>> sccs;

        unordered_map<u_int32_t, u_int32_t> preorder;
        unordered_map<u_int32_t, u_int32_t> lowlink;
        unordered_set<u_int32_t> scc_found;
        vector<u_int32_t> scc_queue;
        vector<u_int32_t> queue;

        u_int64_t i = 0;

        u_int32_t unitigIndex_source = nodeIndex_to_unitigIndex(nodeIndex_source);

        queue.clear();
        queue.push_back(nodeIndex_source);

        while(queue.size() > 0){
            u_int32_t v = queue[queue.size()-1];

            if(preorder.find(v) == preorder.end()){
                i += 1;
                preorder[v] = i;
            }

            bool done = true;

            vector<u_int32_t> v_nbrs;
            if(forward){
                getSuccessors(v, 0, v_nbrs); //_unitigs[v]._endNode
            }
            else{
                getPredecessors(v, 0, v_nbrs); //_unitigs[v]._endNode
            }
            
            //_logFile << BiGraph::nodeIndex_to_nodeName(v) << " " << v_nbrs.size() << endl;
            //getchar();

            for(u_int32_t w : v_nbrs){
                if(preorder.find(w) == preorder.end()){
                    queue.push_back(w);
                    done = false;
                    break;
                }
            }

            if(done){
                lowlink[v] = preorder[v];
                for(u_int32_t w : v_nbrs){
                    if(scc_found.find(w) == scc_found.end()){
                        if(preorder[w] > preorder[v]){
                            lowlink[v] = min(lowlink[v], lowlink[w]);
                        }
                        else{
                            lowlink[v] = min(lowlink[v], preorder[w]);
                        }
                    }
                }

                queue.pop_back();

                if(lowlink[v] == preorder[v]){
                    //_logFile << "----" << endl;
                    scc_found.insert(v);
                    vector<u_int32_t> scc = {v};

                    bool isSourceScc = false;

                    if(nodeIndex_to_unitigIndex(v) == unitigIndex_source) isSourceScc = true;

                    while(scc_queue.size() > 0 && preorder[scc_queue[scc_queue.size()-1]] > preorder[v]){
                        u_int32_t k = scc_queue[scc_queue.size()-1];
                        scc_queue.pop_back();
                        scc_found.insert(k);
                        scc.push_back(k);

                        //_logFile << "Lala: " << unitigIndex_source << " " << k << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[k]._startNode) << endl;

                        if(nodeIndex_to_unitigIndex(k) == unitigIndex_source) isSourceScc = true;
                    }

                    if(isSourceScc){
                        for(u_int32_t nodeIndex : scc){
                            u_int32_t unitigIndex = nodeIndex_to_unitigIndex(nodeIndex);
                            if(std::find(component.begin(), component.end(), unitigIndex) != component.end()) continue;
                            component.push_back(unitigIndex);
                        }
                        //_logFile << "found" << endl;
                        //component = scc;
                        return;
                    }
                    
                }
                else{
                    scc_queue.push_back(v);
                }
            }
        }
        

    }

    void getStronglyConnectedComponents_node_2(u_int32_t nodeIndex_source, bool forward, vector<vector<u_int32_t>>& components){ //, vector<u_int32_t>& scc
        //scc.clear();
    
        components.clear();
        //vector<vector<u_int32_t>> sccs;

        unordered_map<u_int32_t, u_int32_t> preorder;
        unordered_map<u_int32_t, u_int32_t> lowlink;
        unordered_set<u_int32_t> scc_found;
        vector<u_int32_t> scc_queue;
        vector<u_int32_t> queue;

        u_int64_t i = 0;

        u_int32_t unitigIndex_source = nodeIndex_to_unitigIndex(nodeIndex_source);

        queue.clear();
        queue.push_back(nodeIndex_source);

        while(queue.size() > 0){
            u_int32_t v = queue[queue.size()-1];

            if(preorder.find(v) == preorder.end()){
                i += 1;
                preorder[v] = i;
            }

            bool done = true;

            vector<u_int32_t> v_nbrs;
            if(forward){
                getSuccessors(v, 0, v_nbrs); //_unitigs[v]._endNode
            }
            else{
                getPredecessors(v, 0, v_nbrs); //_unitigs[v]._endNode
            }
            
            //_logFile << BiGraph::nodeIndex_to_nodeName(v) << " " << v_nbrs.size() << endl;
            //getchar();

            for(u_int32_t w : v_nbrs){
                if(preorder.find(w) == preorder.end()){
                    queue.push_back(w);
                    done = false;
                    break;
                }
            }

            if(done){
                lowlink[v] = preorder[v];
                for(u_int32_t w : v_nbrs){
                    if(scc_found.find(w) == scc_found.end()){
                        if(preorder[w] > preorder[v]){
                            lowlink[v] = min(lowlink[v], lowlink[w]);
                        }
                        else{
                            lowlink[v] = min(lowlink[v], preorder[w]);
                        }
                    }
                }

                queue.pop_back();

                if(lowlink[v] == preorder[v]){
                    //_logFile << "----" << endl;
                    scc_found.insert(v);
                    vector<u_int32_t> scc = {v};

                    bool isSourceScc = false;

                    if(nodeIndex_to_unitigIndex(v) == unitigIndex_source) isSourceScc = true;

                    while(scc_queue.size() > 0 && preorder[scc_queue[scc_queue.size()-1]] > preorder[v]){
                        u_int32_t k = scc_queue[scc_queue.size()-1];
                        scc_queue.pop_back();
                        scc_found.insert(k);
                        scc.push_back(k);

                        //_logFile << "Lala: " << unitigIndex_source << " " << k << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[k]._startNode) << endl;

                        if(nodeIndex_to_unitigIndex(k) == unitigIndex_source) isSourceScc = true;
                    }

                    vector<u_int32_t> component;
                    //if(isSourceScc){
                        for(u_int32_t nodeIndex : scc){
                            u_int32_t unitigIndex = nodeIndex_to_unitigIndex(nodeIndex);
                            if(std::find(component.begin(), component.end(), unitigIndex) != component.end()) continue;
                            component.push_back(unitigIndex);
                        }
                        //_logFile << "found" << endl;
                        //component = scc;
                        //return;
                    //}
                    components.push_back(component);
                }
                else{
                    scc_queue.push_back(v);
                }
            }
        }
        

    }

    void getStronglyConnectedComponents_node(u_int32_t nodeIndex_source, bool forward, vector<vector<u_int32_t>>& components){ //, vector<u_int32_t>& scc
        //scc.clear();
    
        vector<vector<u_int32_t>> components_tmp;
        components.clear();
        //vector<vector<u_int32_t>> sccs;

        unordered_map<u_int32_t, u_int32_t> preorder;
        unordered_map<u_int32_t, u_int32_t> lowlink;
        unordered_set<u_int32_t> scc_found;
        vector<u_int32_t> scc_queue;
        vector<u_int32_t> queue;

        u_int64_t i = 0;

        //u_int32_t unitigIndex_source = nodeIndex_to_unitigIndex(nodeIndex_source);

        queue.clear();
        queue.push_back(nodeIndex_source);

        while(queue.size() > 0){
            u_int32_t v = queue[queue.size()-1];

            if(preorder.find(v) == preorder.end()){
                i += 1;
                preorder[v] = i;
            }

            bool done = true;

            vector<u_int32_t> v_nbrs;
            if(forward){
                getSuccessors(v, 0, v_nbrs); //_unitigs[v]._endNode
            }
            else{
                getPredecessors(v, 0, v_nbrs); //_unitigs[v]._endNode
            }
            
            //_logFile << BiGraph::nodeIndex_to_nodeName(v) << " " << v_nbrs.size() << endl;
            //getchar();

            for(u_int32_t w : v_nbrs){
                if(preorder.find(w) == preorder.end()){
                    queue.push_back(w);
                    done = false;
                    break;
                }
            }

            if(done){
                lowlink[v] = preorder[v];
                for(u_int32_t w : v_nbrs){
                    if(scc_found.find(w) == scc_found.end()){
                        if(preorder[w] > preorder[v]){
                            lowlink[v] = min(lowlink[v], lowlink[w]);
                        }
                        else{
                            lowlink[v] = min(lowlink[v], preorder[w]);
                        }
                    }
                }

                queue.pop_back();

                if(lowlink[v] == preorder[v]){
                    //_logFile << "----" << endl;
                    scc_found.insert(v);
                    vector<u_int32_t> scc = {v};

                    bool isSourceScc = false;

                    if(v == nodeIndex_source) isSourceScc = true;

                    while(scc_queue.size() > 0 && preorder[scc_queue[scc_queue.size()-1]] > preorder[v]){
                        u_int32_t k = scc_queue[scc_queue.size()-1];
                        scc_queue.pop_back();
                        scc_found.insert(k);
                        scc.push_back(k);

                        //_logFile << "Lala: " << unitigIndex_source << " " << k << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[k]._startNode) << endl;

                        if(k == nodeIndex_source) isSourceScc = true;
                    }

                    if(isSourceScc){
                        //_logFile << "found" << endl;
                        //component = scc;
                        //return;
                    }
                    components_tmp.push_back(scc);
                }
                else{
                    scc_queue.push_back(v);
                }
            }
        }
        
        //unordered_map<u_int32_t, u_int32_t> unitigTo;
        unordered_map<u_int32_t, vector<u_int32_t>> unitigToNodes;

        for(vector<u_int32_t>& scc : components_tmp){
            for(u_int32_t nodeIndex : scc){
                
                u_int32_t unitigIndex = nodeIndex_to_unitigIndex(nodeIndex);
                unitigToNodes[unitigIndex].push_back(nodeIndex);
            }
        }


        //unordered_map<u_int32_t, u_int32_t> unitigToScc;
        unordered_set<u_int32_t> nodeVisited;


        for(vector<u_int32_t>& scc : components_tmp){

            vector<u_int32_t> sccNew;
            
            for(u_int32_t nodeIndex : scc){
                if(nodeVisited.find(nodeIndex) != nodeVisited.end()) continue;

                u_int32_t unitigIndex = nodeIndex_to_unitigIndex(nodeIndex);
                for(u_int32_t n : unitigToNodes[unitigIndex]){
                    sccNew.push_back(n);
                    nodeVisited.insert(n);
                }
            }

            if(sccNew.size() == 0) continue;

            components.push_back(sccNew);

        }

    }

    u_int32_t shortestPath(u_int32_t source_nodeIndex, u_int32_t sink_nodeIndex, vector<u_int32_t>& path, bool includeSource, bool includeSink, bool forward){

        path.clear();
		unordered_set<u_int32_t> isVisited;
		unordered_map<u_int32_t, u_int32_t> distance;
		unordered_map<u_int32_t, u_int32_t> prev;
        queue<u_int32_t> queue;


        distance[source_nodeIndex] = 0;
        isVisited.insert(source_nodeIndex);
		prev[source_nodeIndex] = -1;

        queue.push(source_nodeIndex);
        bool found = false;

        while (!queue.empty() && !found){

            u_int32_t nodeIndex_current = queue.front();
            queue.pop();

			vector<u_int32_t> successors;
            if(forward){
			    getSuccessors(nodeIndex_current, 0, successors);
            }
            else{
			    getPredecessors(nodeIndex_current, 0, successors);
            }


			for(u_int32_t nodeIndex_successor : successors){

                if (isVisited.find(nodeIndex_successor) != isVisited.end()) continue;

                distance[nodeIndex_successor] = distance[nodeIndex_current] + 1;
                queue.push(nodeIndex_successor);
                isVisited.insert(nodeIndex_successor);
                prev[nodeIndex_successor] = nodeIndex_current;

                if(nodeIndex_successor == sink_nodeIndex){
                    found = true;
                    break;
                }

            }



        }

        if(found){

            u_int32_t n = sink_nodeIndex;
            while(n != source_nodeIndex){
                if(n == sink_nodeIndex){
                    if(includeSink) path.push_back(n);
                }
                else{
                    path.push_back(n);
                }

                n = prev[n];
            }
            if(includeSource) path.push_back(source_nodeIndex);

            return distance[sink_nodeIndex];
        }

        return -1;
    }
    */
    /*
    u_int32_t shortestPath_unitig(u_int32_t source_unitigIndex, u_int32_t sink_unitigIndex, vector<u_int32_t>& path, bool includeSource, bool includeSink){

        path.clear();
		unordered_set<u_int32_t> isVisited;
		unordered_map<u_int32_t, u_int32_t> distance;
		unordered_map<u_int32_t, u_int32_t> prev;
        queue<u_int32_t> queue;


        distance[source_unitigIndex] = 0;
        isVisited.insert(source_unitigIndex);
		prev[source_unitigIndex] = -1;

        queue.push(source_unitigIndex);
        bool found = false;

        while (!queue.empty() && !found){

            u_int32_t unitigIndex_current = queue.front();
            queue.pop();

			vector<u_int32_t> successors;
			getSuccessors_unitig(unitigIndex_current, 0, successors);


			for(u_int32_t unitigIndex_successor : successors){

                if (isVisited.find(unitigIndex_successor) != isVisited.end()) continue;

                distance[unitigIndex_successor] = distance[unitigIndex_current] + 1;
                queue.push(unitigIndex_successor);
                isVisited.insert(unitigIndex_successor);
                prev[unitigIndex_successor] = unitigIndex_current;

                if(unitigIndex_successor == sink_unitigIndex){
                    found = true;
                    break;
                }

            }



        }

        if(found){

            u_int32_t n = sink_unitigIndex;
            while(n != source_unitigIndex){
                if(n == sink_unitigIndex){
                    if(includeSink) path.push_back(n);
                }
                else{
                    path.push_back(n);
                }

                n = prev[n];
            }
            if(includeSource) path.push_back(source_unitigIndex);

            return distance[sink_unitigIndex];
        }

        return -1;
    }
    */

    void collectNodes_betweenSourceSink_unitig(u_int32_t source_unitigIndex, u_int32_t sink_unitigIndex, vector<u_int32_t>& nodes, float abundanceCutoff_min, const vector<UnitigData>& _unitigDatas, bool forward){


        //u_int32_t source_nodeName = BiGraph::nodeIndex_to_nodeName(_unitigs[source_unitigIndex]._startNode);

        nodes.clear();

		unordered_set<u_int32_t> isVisited;
        queue<u_int32_t> queue;

        isVisited.insert(source_unitigIndex);
        isVisited.insert(sink_unitigIndex);
        queue.push(source_unitigIndex);

        while (!queue.empty()){

            u_int32_t unitigIndex_current = queue.front();
            queue.pop();

			vector<u_int32_t> successors;
            if(forward){
			    getSuccessors_unitig(unitigIndex_current, 0, successors);
            }
            else{
			    getPredecessors_unitig(unitigIndex_current, 0, successors);
            }

            //_logFile << BiGraph::nodeToString(_unitigs[unitigIndex_current]._endNode) << " " << unitigIndex_current << " " << successors.size() << endl;
			for(u_int32_t unitigIndex_successor : successors){

                //_logFile << unitigIndex_successor << " " << (isVisited.find(unitigIndex_successor) != isVisited.end()) << endl;
                if (isVisited.find(unitigIndex_successor) != isVisited.end()) continue;

                /*
                if(abundanceCutoff_min > 0){
                    u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_successor]._startNode);
                    u_int64_t nbSharedReads = Utils::computeSharedReads(_unitigDatas[source_nodeName], _unitigDatas[nodeName]);
                    //_logFile << nodeName_neighbor << " " << nbSharedReads << endl;
                    if(nbSharedReads ==0 || nbSharedReads < abundanceCutoff_min) continue;
                }*/

                queue.push(unitigIndex_successor);
                isVisited.insert(unitigIndex_successor);
                nodes.push_back(unitigIndex_successor);
                //_logFile << "\t\tCollected node: " <<  BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex_successor]._startNode) << endl;

            }
        }

    }

    /*
    unordered_set<u_int32_t> _allowedNodeIndex;
    void disconnectSubGraph(u_int32_t source_unitigIndex, u_int64_t maxLength, bool forward){

        _allowedNodeIndex.clear();
        //u_int32_t source_nodeName = BiGraph::nodeIndex_to_nodeName(source_nodeIndex);
        //u_int32_t source_unitigIndex;

        _isNodeInvalid_tmp.clear();

		vector<u_int32_t> predecessors;
        if(forward){
            //getPredecessors(source_nodeIndex, 0, predecessors);
		    getPredecessors_unitig(source_unitigIndex, 0, predecessors);
        }
        else{
            //getSuccessors(source_nodeIndex, 0, predecessors);
		    getSuccessors_unitig(source_unitigIndex, 0, predecessors);
        }

		for(u_int32_t unitigIndex : predecessors){
            disconnectUnitig(unitigIndex);
		}


		//ofstream file_scc("/home/gats/workspace/run/overlap_test_AD/subgraph.csv");
		//file_scc << "Name,Colour" << endl;

        unordered_set<u_int32_t> isVisited;
        //unordered_set<u_int32_t> visitedUnitigs;
        queue <u_int32_t> queue;
        unordered_map<u_int32_t, u_int32_t> distance;

        queue.push(source_unitigIndex);
        //isVisited.insert(source_unitigIndex);
        distance[source_unitigIndex] = 0;
        //unordered_set<u_int32_t> validUnitigIndex;

        while (!queue.empty()){

            u_int64_t unitigIndex = queue.front();
            //file_scc << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "red" << endl;
            queue.pop();

            if(distance[unitigIndex] > maxLength) continue;

            if (isVisited.find(unitigIndex) != isVisited.end()) continue;
            isVisited.insert(unitigIndex);

            vector<u_int32_t> neighbors;
            if(forward){
                getSuccessors_unitig(unitigIndex, 0, neighbors);
            }
            else{
                getPredecessors_unitig(unitigIndex, 0, neighbors);
            }

            //_logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << " " << neighbors.size() << endl;
            
            for(u_int32_t& unitigIndex_nn : neighbors){

                //u_int32_t nodeIndex_neighbor = node_neighbor._index;
                //u_int32_t nodeName_neighbor = BiGraph::nodeIndex_to_nodeName(nodeIndex_neighbor);

                //_logFile << nodeName_neighbor << " " << (isVisited.find(nodeIndex_neighbor) != isVisited.end()) << " " << distance[nodeIndex_neighbor] << endl;


                //_logFile << "2: " << _nodeToUnitig[nodeIndex_neighbor] << endl;
                //u_int32_t unitigIndex_neighbor = nodeIndex_to_unitigIndex(nodeIndex_neighbor);

                //float overlapLength = 0;
                //if(forward){
                //    overlapLength = _graphSuccessors->getOverlap(_unitigs[unitigIndex]._endNode, _unitigs[unitigIndex_nn]._startNode);
                //}
                //else{
                //    overlapLength = _graphSuccessors->getOverlap(_unitigs[unitigIndex]._startNode, _unitigs[unitigIndex_nn]._endNode);
                //}

                distance[unitigIndex_nn] = distance[unitigIndex] + _unitigs[unitigIndex_nn]._length - _kminmerOverlapMean; //(_nodeLengths[BiGraph::nodeIndex_to_nodeName(nodeIndex_neighbor)] - node_neighbor._overlap);

                //_logFile << nodeName_neighbor << " " << distance[nodeIndex_neighbor] << endl;
                
                //if(abundanceCutoff_min > 0){
				  //   u_int64_t nbSharedReads = Utils::computeSharedReads(_unitigDatas[source_nodeName], _unitigDatas[nodeName_neighbor]);
                    //_logFile << nodeName_neighbor << " " << nbSharedReads << endl;
                    //if(nbSharedReads == 0 || nbSharedReads < abundanceCutoff_min) continue;
                //}

                queue.push(unitigIndex_nn);
                //visitedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex_neighbor));
                //visitedUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex_toReverseDirection(nodeIndex_neighbor)));
                
                //validUnitigIndex.insert(unitigIndex_nn;)
                //_allowedNodeIndex.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex));
                //_logFile << "Visited: " << BiGraph::nodeToString(nodeIndex_neighbor) << " " << nodeIndex_to_unitigIndex(nodeIndex_neighbor) << "             " << distance[nodeIndex_neighbor] << "    " << _nodeLengths[BiGraph::nodeIndex_to_nodeName(nodeIndex_neighbor)] << " " << node_neighbor._overlap << endl;
                //component.push_back(nodeIndex_neighbor);
            }

        }

        for(u_int32_t unitigIndex : isVisited){

            //_allowedNodeIndex.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex));
            
            //_logFile << "miam: " << unitigIndex << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._startNode) << endl;
            //_logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._endNode) << endl;

            vector<u_int32_t> successors;
            if(forward){
                getSuccessors_unitig(unitigIndex, 0, successors);
            }
            else{
                getPredecessors_unitig(unitigIndex, 0, successors);
            }

            for(u_int32_t successor_unitigIndex : successors){
                //u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(nodeIndex);
                if(isVisited.find(successor_unitigIndex) != isVisited.end()) continue; 
                if(isVisited.find(unitigIndex_toReverseDirection(successor_unitigIndex)) != isVisited.end()) continue; 

                //_logFile << unitigIndex << " " << (visitedUnitigs.find(unitigIndex) != visitedUnitigs.end()) << endl;
                //if(visitedUnitigs.find(nodeIndex) != visitedUnitigs.end()) continue; 
                //if(isVisited.find(_unitigs[unitigIndex]._startNode) != isVisited.end() ||  isVisited.find(_unitigs[unitigIndex]._endNode) != isVisited.end()) continue;
                //if(isVisited.find(nodeIndex_toReverseDirection(_unitigs[unitigIndex]._startNode)) != isVisited.end() ||  isVisited.find(nodeIndex_toReverseDirection(_unitigs[unitigIndex]._endNode)) != isVisited.end()) continue;
                //_logFile << BiGraph::nodeToString(_unitigs[unitigIndex]._startNode) << " " << BiGraph::nodeToString(_unitigs[unitigIndex]._endNode) << endl;
                disconnectUnitig(successor_unitigIndex);
            }
        }

        //_allowedNodeIndex = isVisited;
        //file_scc.close();
        
        //exit(1);
    }

    void disconnectUnitig(u_int32_t unitigIndex){

        _isNodeInvalid_tmp.insert(unitigIndex);
        _isNodeInvalid_tmp.insert(nodeIndex_toReverseDirection(unitigIndex));
        //_logFile << "Disconnected: " << BiGraph::nodeIndex_to_nodeName(unitigIndex) << endl;
    }

    void extractSubGraph(u_int32_t source_nodeIndex, vector<u_int32_t>& successor_nodeIndex, u_int64_t maxLength, bool forward){

        //_logFile << BiGraph::nodeIndex_to_nodeName(source_nodeIndex) << endl;
		ofstream file_scc("/home/gats/workspace/run/overlap_test_AD/subgraph.csv");
		file_scc << "Name,Colour" << endl;



        //u_int32_t source_nodeName = BiGraph::nodeIndex_to_nodeName(source_nodeIndex);

        _isNodeInvalid_tmp.clear();

        unordered_set<u_int32_t> isVisited;
        queue <u_int32_t> queue;
        unordered_map<u_int32_t, u_int32_t> distance;

        queue.push(source_nodeIndex);
        isVisited.insert(source_nodeIndex);
        distance[source_nodeIndex] = 0;

        while (!queue.empty()){

            u_int64_t nodeIndex = queue.front();

            queue.pop();

            vector<AdjNode> neighbors;
            //vector<AdjNode> successors;
            //vector<AdjNode> predecessors;
            if(forward){
                getSuccessors_overlap(nodeIndex, 0, neighbors);
            }
            else{
                getPredecessors_overlap(nodeIndex, 0, neighbors);
            }

            //for(AdjNode& node : successors) neighbors.push_back(node);
            //for(AdjNode& node : predecessors) neighbors.push_back(node);

            
            for(AdjNode& node_neighbor : neighbors){

                u_int32_t nodeIndex_neighbor = node_neighbor._index;
                if (isVisited.find(nodeIndex_neighbor) != isVisited.end()) continue;

                distance[nodeIndex_neighbor] = distance[nodeIndex] + (_graphSuccessors->_nodeDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex_neighbor)]._length - _kminmerOverlapMean);

                if(distance[nodeIndex_neighbor] > maxLength) continue;

                queue.push(nodeIndex_neighbor);
                isVisited.insert(nodeIndex_neighbor);
            }

        }


        for(u_int32_t nodeIndex : isVisited){
            file_scc << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "red" << endl;
        }

        file_scc.close();
    }

    void selectSubGraph(u_int32_t source_nodeIndex, u_int64_t maxLength, bool forward){

        unordered_set<u_int32_t> successorUnitigs;

		vector<u_int32_t> successors;
        if(forward){
            getSuccessors(source_nodeIndex, 0, successors);
        }
        else{
            getPredecessors(source_nodeIndex, 0, successors);
        }

        for(u_int32_t nodeIndex : successors){
            successorUnitigs.insert(nodeIndex_to_unitigIndex(nodeIndex));
            successorUnitigs.insert(unitigIndex_toReverseDirection(nodeIndex_to_unitigIndex(nodeIndex)));
        }

        //unordered_set<u_int32_t> validNodeIndex;
        //_allowedNodeIndex.clear();
        //u_int32_t source_nodeName = BiGraph::nodeIndex_to_nodeName(source_nodeIndex);

		ofstream file_scc("/home/gats/workspace/run/subgraph.csv");
		file_scc << "Name,Colour" << endl;

        unordered_set<u_int32_t> isVisited;
        //unordered_set<u_int32_t> isVisitedUnitig;
        //unordered_set<u_int32_t> visitedUnitigs;
        queue <u_int32_t> queue;
        unordered_map<u_int32_t, u_int32_t> distance;

        queue.push(source_nodeIndex);
        isVisited.insert(source_nodeIndex);
        distance[source_nodeIndex] = 0;

        _isBoundUnitig.clear();
        _isBoundUnitig.insert(nodeIndex_to_unitigIndex(source_nodeIndex));
        _isBoundUnitig.insert(unitigIndex_toReverseDirection(nodeIndex_to_unitigIndex(source_nodeIndex)));
        
        while (!queue.empty()){

            u_int64_t nodeIndex = queue.front();
            queue.pop();

            vector<AdjNode> neighbors;
            if(forward){
                getSuccessors_overlap(nodeIndex, 0, neighbors);
            }
            else{
                getPredecessors_overlap(nodeIndex, 0, neighbors);
            }

            //_logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << " " << neighbors.size() << endl;
            
            for(AdjNode& node_neighbor : neighbors){

                u_int32_t nodeIndex_neighbor = node_neighbor._index;
                //u_int32_t nodeName_neighbor = BiGraph::nodeIndex_to_nodeName(nodeIndex_neighbor);

                //_logFile << nodeName_neighbor << " " << (isVisited.find(nodeIndex_neighbor) != isVisited.end()) << " " << distance[nodeIndex_neighbor] << endl;


                //_logFile << "2: " << _nodeToUnitig[nodeIndex_neighbor] << endl;
                //u_int32_t unitigIndex_neighbor = nodeIndex_to_unitigIndex(nodeIndex_neighbor);
                if (isVisited.find(nodeIndex_neighbor) != isVisited.end()) continue;

                distance[nodeIndex_neighbor] = distance[nodeIndex] + (_graphSuccessors->_nodeDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex_neighbor)]._length - _kminmerOverlapMean);

                //_logFile << nodeName_neighbor << " " << distance[nodeIndex_neighbor] << endl;
                if(distance[nodeIndex_neighbor] > maxLength){
                    if(successorUnitigs.find(nodeIndex_to_unitigIndex(nodeIndex_neighbor)) == successorUnitigs.end()){
                        _isBoundUnitig.insert(nodeIndex_to_unitigIndex(nodeIndex_neighbor));
                        _isBoundUnitig.insert(nodeIndex_to_unitigIndex(nodeIndex_toReverseDirection(nodeIndex_neighbor)));

                        for(u_int32_t n : nodeIndex_to_unitig(nodeIndex_neighbor)._nodes){
                            file_scc << BiGraph::nodeIndex_to_nodeName(n) << "," << "red" << endl;
                        }
                    }
                    
                    continue;
                }
                

                queue.push(nodeIndex_neighbor);
                isVisited.insert(nodeIndex_neighbor);
            }

        }



        //_allowedNodeIndex = isVisited;
        file_scc.close();

    }



    void extractReadpathSubgraph(u_int32_t source_unitigIndex){

        //_logFile << "-----" << endl;
        const UnitigData& source_readIndexes = _unitigDatas2[source_unitigIndex];

		ofstream file_scc("/home/gats/workspace/run/subgraph.csv");
		file_scc << "Name,Colour" << endl;

        unordered_set<u_int32_t> isVisited;
        queue<u_int32_t> queue;

        queue.push(source_unitigIndex);

        while (!queue.empty()){

            u_int64_t unitigIndex = queue.front();
            queue.pop();

            if (isVisited.find(unitigIndex) != isVisited.end()) continue;
            isVisited.insert(unitigIndex);

            const UnitigData& readIndexes = _unitigDatas2[unitigIndex];
            //_logFile << _unitigDatas2[unitigIndex]._readIndexes.size() << endl;

            u_int32_t nbSharedReads = Utils::computeSharedReads(source_readIndexes, readIndexes);
            //_logFile << nbSharedReads << endl;
            //_logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._endNode) << " " << nbSharedReads << endl;
            //_logFile << "Shared reads: " << nbSharedReads << " " << readIndexes._readIndexes.size() << " " << source_readIndexes._readIndexes.size() << endl;
            if(nbSharedReads <= 1) continue;

            for(u_int32_t nodeIndex : _unitigs[unitigIndex]._nodes){
                file_scc << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "red" << endl;
            }

            vector<u_int32_t> successors;
            getSuccessors_unitig(unitigIndex, 0, successors);
            
            for(u_int32_t unitigIndex_nn : successors){
                queue.push(unitigIndex_nn);
            }

            vector<u_int32_t> predecessors;
            getPredecessors_unitig(unitigIndex, 0, predecessors);
            
            for(u_int32_t unitigIndex_nn : predecessors){
                queue.push(unitigIndex_nn);
            }

        }

        file_scc.close();

    }
    */

    bool isEdgeNode(u_int32_t nodeIndex){
		Unitig& unitig = nodeIndex_to_unitig(nodeIndex);
        return nodeIndex == unitig._startNode || nodeIndex == unitig._endNode;
    }

    /*
    bool isNeighborOfEdgeNode(u_int32_t nodeIndex){

        vector<u_int32_t> successors;
        getSuccessors(nodeIndex, 0, successors);

        for(u_int32_t nodeIndexSuccessor : successors){
            if(isEdgeNode(nodeIndexSuccessor)) return true;
        }

        vector<u_int32_t> predecessors;
        getPredecessors(nodeIndex, 0, predecessors);

        for(u_int32_t nodeIndexPredecessor : predecessors){
            if(isEdgeNode(nodeIndexPredecessor)) return true;
        }

		return false;
    }
    */

    u_int64_t getChecksumNodeNames(){

        u_int64_t sum = 0;

        for(u_int32_t nodeIndex=0; nodeIndex<_isNodeRemoved.size(); nodeIndex++){
            if(_isNodeRemoved[nodeIndex]) continue;
            sum += nodeIndex;
        }

        return sum;
    }

    u_int64_t getChecksumGlobal(){

        u_int64_t sum = 0;

        for(u_int32_t nodeIndex=0; nodeIndex<_isNodeRemoved.size(); nodeIndex++){
            if(_isNodeRemoved[nodeIndex]) continue;
            sum += nodeIndex;
        }

        return sum;
    }

    double getChecksumGlobal_abundance(){

        double sum = 0;

        for(u_int32_t nodeIndex=0; nodeIndex<_isNodeRemoved.size(); nodeIndex++){
            if(_isNodeRemoved[nodeIndex]) continue;
            //sum += _graphSuccessors->_nodeAbundances[BiGraph::nodeIndex_to_nodeName(nodeIndex)];
            sum += getNodeUnitigAbundance(nodeIndex);
        }

        return sum;
    }

    double getChecksumGlobal_abundanceUtg(){

        double sum = 0;

        for(const Unitig& u: _unitigs){
            if(u._startNode == -1) continue;
            sum += u._abundance;
        }

        return sum;
    }

       
    double getChecksumGlobal_utg(){

        double sum = 0;

        for(const Unitig& u: _unitigs){
            if(u._startNode == -1) continue;

            double s=0;

            for(u_int32_t nodeIndex : u._nodes){
                s += nodeIndex;
            }

            sum += s*u._nbNodes;

        }

        return sum;
    }  

    double getChecksumGlobal_nbUtg(unordered_set<u_int32_t>& removedNodes){

        double sum = 0;

        for(const Unitig& u: _unitigs){
            if(u._startNode == -1) continue;

            bool isValid = true;
            for(u_int32_t nodeIndex : u._nodes){
                if(removedNodes.find(nodeIndex) != removedNodes.end() || removedNodes.find(nodeIndex_toReverseDirection(nodeIndex)) != removedNodes.end()){
                    isValid = false;
                }
                break;
            }

            if(!isValid) continue;

            sum += 1;
        }

        return sum;
    }  

    /*
    struct ShortedPathData{
        u_int32_t _unitigIndex;
        u_int32_t _distance;
        //u_int32_t _prevNodeIndex;
    };

    struct ShortedPathData_Comparator {
        bool operator()(ShortedPathData const& p1, ShortedPathData const& p2){
            return p1._distance > p2._distance;
        }
    };
    
    void shortestPath_weighted(u_int32_t source_unitigIndex, u_int32_t sink_unitigIndex, vector<u_int32_t>& path){
        
        path.clear();
        bool found = false;

        unordered_map<u_int32_t, u_int32_t> distances;

        priority_queue< ShortedPathData, vector <ShortedPathData> , ShortedPathData_Comparator> pq;

        pq.push({src, 0, {src}});
        distances[src] = 0;

        while (!pq.empty() && !found)
        {
            
            ShortedPathData pathData = pq.top();
            pq.pop();

            u_int32_t u = pathData._nodeIndex;


            vector<AdjNode> nodes;
            for(AdjNode& node : _nodes[BiGraph::nodeName_to_nodeIndex(u, true)]){
                nodes.push_back(node);
            }
            for(AdjNode& node : _nodes[BiGraph::nodeName_to_nodeIndex(u, false)]){
                nodes.push_back(node);
            }


            //_logFile << graphInfo->id_to_name(u) << " " << nodes.size() << endl;

            for(AdjNode& node : nodes){

                u_int32_t v = BiGraph::nodeIndex_to_nodeName(node._index);
                u_int32_t weight = unitigLengths[v] - node._overlap;

                u_int32_t dist = -1;
                if(distances.find(v) != distances.end()) dist = distances[v];

                if (dist > distances[u] + weight)
                {
                    distances[v] = distances[u] + weight;
                    vector<u_int32_t> p = pathData._path;
                    p.push_back(v);

                    if(v == dest){
                        path.clear();
                        path = p;
                        found = true;
                        break;
                    }

                    //_logFile << "\tAdd node: " << graphInfo->id_to_name(v) << " " << distances[v] << endl;
                    pq.push({v, distances[v], p});
                }

            }

        }

        //_logFile << path.size() << endl;
        //printf("Vertex Distance from Source\n");
        //for (int i = 0; i < V; ++i)
        //    printf("%d \t\t %d\n", i, distance[i]);
    }
    */
    /*
    ofstream file_scc;

    void shortestPath_unitig_weighted(u_int32_t source_unitigIndex, unordered_map<u_int32_t, vector<u_int32_t>>& destPaths, bool includeSource, bool includeSink, u_int32_t pass){

        
        //_logFile << "-----" << endl;

        unordered_set<u_int32_t> dests;
        for(const auto& destPath : destPaths){
            dests.insert(destPath.first);
            //_logFile << source_unitigIndex << " -> " << destPath.first << endl;
        }
        //getchar();
       
        //if(pass == 60){
        //    file_scc.open("/home/gats/workspace/run/scc.csv");
		//	file_scc << "Name,Colour" << endl;
        //}
        //u_int32_t sink_unitigIndex_rev = unitigIndex_toReverseDirection(source_unitigIndex);

        //bool found = false;
        //u_int32_t found_unitigIndex = -1;
        //path.clear();

        priority_queue< ShortedPathData, vector <ShortedPathData> , ShortedPathData_Comparator> pq;
        unordered_map<u_int32_t, u_int32_t> dist;
		unordered_map<u_int32_t, u_int32_t> prev;

        pq.push({source_unitigIndex, 0});
        dist[source_unitigIndex] = 0;
        prev[source_unitigIndex] = -1;
    


        while(!pq.empty()){
            
            int u = pq.top()._unitigIndex;
            pq.pop();
            
            //_logFile << u << endl;
            if(_unitigs[u]._startNode == -1) continue;
            
            if(dests.find(u) != dests.end() || dests.find(unitigIndex_toReverseDirection(u)) != dests.end()){
                dests.erase(u);
            }
            //_logFile << dests.size() << " " << prev.size() << endl;
            if(dests.size() == 0) break;

            //if(pass == 60){
            //    for(u_int32_t nodeIndex : _unitigs[u]._nodes){
            //        file_scc << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "red" << endl;
            //    }
            //}


            //_logFile << u << " " << pq.size() << endl;
            vector<u_int32_t> successors;
            getSuccessors_unitig(u, 0, successors);
            vector<u_int32_t> predecessors;
            getPredecessors_unitig(u, 0, predecessors);

            vector<u_int32_t> neighbors;
            neighbors.insert(neighbors.end(), successors.begin(), successors.end());
            neighbors.insert(neighbors.end(), predecessors.begin(), predecessors.end());


            for(u_int32_t v : neighbors){

                u_int32_t weight = _unitigs[v]._nbNodes;
                
                if(dist.find(v) == dist.end()){
                    dist[v] = -1;
                }

                if(dist[v] > dist[u] + weight){
                    
                    prev[v] = u;

                    u_int32_t unitigIndexDest = -1;

                    if(destPaths.find(v) != destPaths.end()){
                        unitigIndexDest = v;
                    }
                    else if(destPaths.find(unitigIndex_toReverseDirection(v)) != destPaths.end()){
                        unitigIndexDest = unitigIndex_toReverseDirection(v);
                    }

                    if(unitigIndexDest != -1){
                        vector<u_int32_t> path;

                        u_int32_t n = v;
                        while(n != source_unitigIndex){


                            if(n == v){
                                if(includeSink) path.push_back(n);
                            }
                            else{
                                path.push_back(n);
                            }

                            n = prev[n];
                        }
                        if(includeSource) path.push_back(source_unitigIndex);

                        destPaths[unitigIndexDest] = path;

                    }
                    //if(v == sink_unitigIndex || v == sink_unitigIndex_rev){
                    //    found_unitigIndex = v;
                    //    found = true;
                    //    break;
                    //}

                    dist[v] = dist[u] + weight;
                    pq.push({v, dist[v]});
                }
            }

        }

        //if(pass == 60){
        //    _logFile << "debug done" << endl;
        //    file_scc.close();
        //    getchar();
        //}


    }
    */

    /*
    void determineRepeats(const vector<UnitigData>& unitigDatas, float abundanceCutoff){

        u_int32_t minSupportingReads = abundanceCutoff / 4;
        if(minSupportingReads < 2) minSupportingReads = 2;

        _logFile << "Detecting repeat" << endl;
        _logFile << "Min supporting reads: " << minSupportingReads << endl;

        u_int32_t unitigIndexLala = nodeIndex_to_unitigIndex(BiGraph::nodeName_to_nodeIndex(1242487, false));
        //_logFile << Utils::computeSharedReads(unitigDatas[12264], unitigDatas[25489]) << endl;
        //_logFile << Utils::computeSharedReads(unitigDatas[4876], unitigDatas[6347]) << endl;
        //exit(1);

        ofstream file_repeat(_outputDir + "/repeat.csv");
        file_repeat << "Name,Color" << endl;

        unordered_set<u_int32_t> isUnitigUnique;
        vector<Unitig> unitigSortedByLength;

        for(const Unitig& unitig: _unitigs){
            //_logFile << unitig._length << " " << unitig._abundance << endl;
            //if(unitig._index % 2 == 1) continue;
            //if(unitig._length < 10000) continue;
            //_logFileif(unitig._length < 50000) continue;
            //if(unitig._length < 10000 || unitig._length > 15000) continue;
            //if(unitig._abundance < 10) continue; //200
            
            unitigSortedByLength.push_back(unitig);
            isUnitigUnique.insert(unitig._index);
        }
        std::sort(unitigSortedByLength.begin(), unitigSortedByLength.end(), UnitigComparator_ByLength_Reverse);

        size_t nbPass = 0;
        while(true){

            if(nbPass >= 5) break;

            for(const Unitig& unitig : unitigSortedByLength){


                unordered_set<u_int32_t> uniqueSuccessors;
                determineRepeat_getNextUniqueSuccessors(unitig._index, true, isUnitigUnique, uniqueSuccessors, unitigDatas, minSupportingReads);

                if(uniqueSuccessors.size() > 1){

                    if(unitig._index == unitigIndexLala || unitigIndex_toReverseDirection(unitig._index) == unitigIndexLala){
                        _logFile << uniqueSuccessors.size() << endl;
                        //getchar();
                    }

                    isUnitigUnique.erase(unitig._index);
                    isUnitigUnique.erase(unitigIndex_toReverseDirection(unitig._index));
                }

                unordered_set<u_int32_t> uniquePredecessors;
                determineRepeat_getNextUniqueSuccessors(unitig._index, false, isUnitigUnique, uniquePredecessors, unitigDatas, minSupportingReads);

                if(uniquePredecessors.size() > 1){

                    if(unitig._index == unitigIndexLala || unitigIndex_toReverseDirection(unitig._index) == unitigIndexLala){
                        _logFile << uniquePredecessors.size() << endl;
                        //getchar();
                    }

                    isUnitigUnique.erase(unitig._index);
                    isUnitigUnique.erase(unitigIndex_toReverseDirection(unitig._index));


                }




            }



            nbPass += 1;
        }

        for(const Unitig& unitig: _unitigs){
            if(isUnitigUnique.find(unitig._index) == isUnitigUnique.end()){
                for(u_int32_t nodeIndex : unitig._nodes){
                    file_repeat << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "red" << endl;
                }
            }
            else{
                for(u_int32_t nodeIndex : unitig._nodes){
                    file_repeat << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "blue" << endl;
                }
            }
        }


        file_repeat.close();
    }

	void determineRepeat_getNextUniqueSuccessors(u_int32_t source_unitigIndex, bool forward, unordered_set<u_int32_t>& isUnitigUnique, unordered_set<u_int32_t>& uniqueSuccessors, const vector<UnitigData>& unitigDatas, u_int32_t minSupportingReads){

        u_int32_t source_nodeIndex = -1;
        if(forward){
            source_nodeIndex = _unitigs[source_unitigIndex]._endNode;
        }
        else{
            source_nodeIndex = _unitigs[unitigIndex_toReverseDirection(source_unitigIndex)]._endNode;
        }

        const vector<u_int64_t>& source_readIndexes = unitigDatas[BiGraph::nodeIndex_to_nodeName(source_nodeIndex)]._readIndexes;

        u_int32_t unitigIndexLala = nodeIndex_to_unitigIndex(BiGraph::nodeName_to_nodeIndex(1242487, false));
        //_logFile << "-----" << endl;

        uniqueSuccessors.clear();
		//const vector<u_int64_t>& source_readIndexes = unitigDatas[source_unitigIndex]._readIndexes;

      	unordered_set<u_int32_t> isVisited;
        queue<u_int32_t> queue;

        queue.push(source_unitigIndex);

        while (!queue.empty()){

            u_int64_t unitigIndex = queue.front();
            queue.pop();

            if (isVisited.find(unitigIndex) != isVisited.end()) continue;
            isVisited.insert(unitigIndex);

            //const vector<u_int64_t>& readIndexes = unitigDatas[unitigIndex]._readIndexes;

			if(unitigIndex != source_unitigIndex && unitigIndex != unitigIndex_toReverseDirection(source_unitigIndex)){

                //u_int64_t nbSharedReads = Utils::computeSharedReads(source_readIndexes, readIndexes);

                const vector<u_int64_t>& readIndexes = unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._startNode)]._readIndexes;
                u_int64_t nbSharedReads = Utils::computeSharedReads(source_readIndexes, readIndexes);
            


              

                //if(source_unitigIndex == unitigIndexLala || unitigIndex_toReverseDirection(source_unitigIndex) == unitigIndexLala){
                //    _logFile << BiGraph::nodeIndex_to_nodeName(source_nodeIndex) << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._startNode) << " " <<  nbSharedReads << endl;
                //    //getchar();
                //}

                if(nbSharedReads < minSupportingReads) continue;



                if(isUnitigUnique.find(unitigIndex) != isUnitigUnique.end()){
                    //_logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._startNode) << " " <<  nbSharedReads << endl;
                    
                    bool isValid = true;
                    for(u_int32_t u : uniqueSuccessors){
                        if(Utils::computeSharedReads(unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._startNode)], unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[u]._startNode)]) > 1){
                            //_logFile << "\tinvalid" << endl;
                            isValid = false;
                            break;
                        }
                        if(Utils::computeSharedReads(unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._startNode)], unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[u]._endNode)]) > 1){
                            //_logFile << "\tinvalid" << endl;
                            isValid = false;
                            break;
                        }
                        if(Utils::computeSharedReads(unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._endNode)], unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[u]._startNode)]) > 1){
                            //_logFile << "\tinvalid" << endl;
                            isValid = false;
                            break;
                        }
                        if(Utils::computeSharedReads(unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._endNode)], unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[u]._endNode)]) > 1){
                            //_logFile << "\tinvalid" << endl;
                            isValid = false;
                            break;
                        }
                    }

                    if(!isValid) continue;

                    uniqueSuccessors.insert(unitigIndex);
                    continue;
                }
			}

            vector<u_int32_t> successors;
            getSuccessors_unitig(unitigIndex, 0, successors);
			//if(forward){
            //	getSuccessors_unitig(unitigIndex, 0, successors);
			//}
			//else{
            //	getPredecessors_unitig(unitigIndex, 0, successors);
			//}
            
            for(u_int32_t unitigIndex_nn : successors){
                queue.push(unitigIndex_nn);
            }



        }

        //file_scc.close();

	}

	void getConnectedComponent_readpath(u_int32_t source_unitigIndex, const vector<UnitigData>& unitigDatas, u_int32_t minSupportingReads, unordered_set<u_int32_t>& component){

        component.clear();

        unordered_set<u_int64_t> readIndexes_unique;
        for(u_int32_t nodeIndex : _unitigs[source_unitigIndex]._nodes){

            const UnitigData& unitigData = unitigDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex)];

            for(u_int64_t readIndex : unitigData._readIndexes){
                readIndexes_unique.insert(readIndex);
            }
        }

        vector<u_int64_t> source_readIndexes;
        for(u_int32_t readIndex : readIndexes_unique){
            source_readIndexes.push_back(readIndex);
        }
        std::sort(source_readIndexes.begin(), source_readIndexes.end());


      	unordered_set<u_int32_t> isVisited;
        queue<u_int32_t> queue;

        queue.push(source_unitigIndex);

        while (!queue.empty()){

            u_int64_t unitigIndex = queue.front();
            queue.pop();

            if (isVisited.find(unitigIndex) != isVisited.end()) continue;
            isVisited.insert(unitigIndex);
            isVisited.insert(unitigIndex_toReverseDirection(unitigIndex));



            unordered_set<u_int64_t> readIndexes_unique2;
            for(u_int32_t nodeIndex : _unitigs[unitigIndex]._nodes){
                for(u_int64_t readIndex : unitigDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex)]._readIndexes){
                    readIndexes_unique2.insert(readIndex);
                }
            }
            vector<u_int64_t> readIndexes;
            for(u_int32_t readIndex : readIndexes_unique2){
                readIndexes.push_back(readIndex);
            }
            std::sort(readIndexes.begin(), readIndexes.end());


            //const vector<u_int64_t>& readIndexes = unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._startNode)]._readIndexes;
            u_int64_t nbSharedReads = Utils::computeSharedReads(source_readIndexes, readIndexes);
        
            if(nbSharedReads < minSupportingReads) continue;

            component.insert(unitigIndex);
            component.insert(unitigIndex_toReverseDirection(unitigIndex));
            //_logFile << "2" << endl;
            vector<u_int32_t> successors;

            getSuccessors_unitig(unitigIndex, 0, successors);
            for(u_int32_t unitigIndex_nn : successors) queue.push(unitigIndex_nn);

            //_logFile << "3" << endl;
            getPredecessors_unitig(unitigIndex, 0, successors);
            for(u_int32_t unitigIndex_nn : successors) queue.push(unitigIndex_nn);

            //_logFile << "4" << endl;
            getSuccessors_unitig(unitigIndex_toReverseDirection(unitigIndex), 0, successors);
            for(u_int32_t unitigIndex_nn : successors) queue.push(unitigIndex_nn);

            //_logFile << "5" << endl;
            getPredecessors_unitig(unitigIndex_toReverseDirection(unitigIndex), 0, successors);
            for(u_int32_t unitigIndex_nn : successors) queue.push(unitigIndex_nn);

        }

        //file_scc.close();

	}
    */
    /*
    u_int64_t countValidSuccessors(unordered_set<u_int32_t>& uniqueSuccessors, const vector<UnitigData>& unitigDatas){

        vector<u_int32_t> succs;
        for(u_int32_t unitigIndex : uniqueSuccessors){
            succs.push_back(unitigIndex);
        }

        u_int64_t n = 0;

        for(size_t i=0; i<succs.size(); i++){

            bool isValid = true;

            for(size_t j=i+1; j<succs.size(); j++){

                _logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[succs[i]]._startNode) << " " << BiGraph::nodeIndex_to_nodeName(_unitigs[succs[j]]._startNode) << " " << isInReadPath(succs[i], succs[j], true, unitigDatas) << " " << isInReadPath(succs[i], succs[j], false, unitigDatas) << endl;
                if(isInReadPath(succs[i], succs[j], true, unitigDatas) || isInReadPath(succs[i], succs[j], false, unitigDatas)){
                    isValid = false;
                    break;
                }
            }

            if(isValid) n += 1;
        }

        return n;
    }
    */

    /*
    u_int64_t countValidSuccessors(unordered_set<u_int32_t>& uniqueSuccessors){

        unordered_set<u_int32_t> addedSuccs;

        for(u_int32_t unitigIndex : uniqueSuccessors){

            bool isValid = true;

            for(u_int32_t unitigIndex_added : addedSuccs){

                if(unitigIndex_added == unitigIndex) continue;

                if(isInReadPath(unitigIndex, unitigIndex_added)){
                    isValid = false;
                    break;
                }
            }

            if(isValid) addedSuccs.insert(unitigIndex);
        }

        _logFile << uniqueSuccessors.size() << " " << addedSuccs.size() << endl;
        return addedSuccs.size();
    }
    */

    /*
	bool isInReadPath(u_int32_t source_unitigIndex, u_int32_t dest_unitigIndex, bool forward, const vector<UnitigData>& unitigDatas){

        u_int32_t source_nodeIndex = -1;
        if(forward){
            source_nodeIndex = _unitigs[source_unitigIndex]._endNode;
        }
        else{
            source_nodeIndex = _unitigs[unitigIndex_toReverseDirection(source_unitigIndex)]._endNode;
        }

        _logFile << "Is in read path: " << BiGraph::nodeIndex_to_nodeName(source_nodeIndex) << endl;

        const vector<u_int64_t>& source_readIndexes = unitigDatas[BiGraph::nodeIndex_to_nodeName(source_nodeIndex)]._readIndexes;

      	unordered_set<u_int32_t> isVisited;
        queue<u_int32_t> queue;

        queue.push(source_unitigIndex);

        while (!queue.empty()){

            u_int64_t unitigIndex = queue.front();
            queue.pop();

            if (isVisited.find(unitigIndex) != isVisited.end()) continue;
            isVisited.insert(unitigIndex);


			if(unitigIndex != source_unitigIndex){

                const vector<u_int64_t>& readIndexes = unitigDatas[BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._startNode)]._readIndexes;
                u_int64_t nbSharedReads = Utils::computeSharedReads(source_readIndexes, readIndexes);
            
                if(nbSharedReads <= 1) continue;

                if(unitigIndex == dest_unitigIndex) return true;
			}

            vector<u_int32_t> successors;
            getSuccessors_unitig(unitigIndex, 0, successors);
            
            for(u_int32_t unitigIndex_nn : successors){
                queue.push(unitigIndex_nn);
            }

        }

        return false;
	}
    */

    /*
    struct NodeSupport{
        u_int32_t _nodeIndex;
        u_int32_t _nbSupportingReads;
    };
    
    struct NodeSupport_Comparator {
        bool operator()(NodeSupport const& p1, NodeSupport const& p2){
            return p1._nbSupportingReads < p2._nbSupportingReads;
        }
    };

    void findBestSupportedPath(u_int32_t source_nodeIndex, u_int32_t dest_nodeName, unordered_set<u_int32_t>& unallowedNodenames, const vector<UnitigData>& unitigDatas, vector<u_int32_t>& nodepath, vector<u_int32_t>& anchorNodeNames){
        

        nodepath.clear();
        const vector<u_int64_t>& readIndexes_source = unitigDatas[BiGraph::nodeIndex_to_nodeName(source_nodeIndex)]._readIndexes;

        vector<u_int32_t> prevNodes;
        vector<u_int32_t> prevNodeNames;
        unordered_set<u_int32_t> isVisited;

        prevNodes.push_back(source_nodeIndex);
        prevNodeNames.push_back(BiGraph::nodeIndex_to_nodeName(source_nodeIndex));
        isVisited.insert(source_nodeIndex);


        for(u_int32_t nodeName : unallowedNodenames){
            isVisited.insert(BiGraph::nodeName_to_nodeIndex(nodeName, true));
            isVisited.insert(BiGraph::nodeName_to_nodeIndex(nodeName, false));
        }

        while(true){
            u_int32_t nodeIndex = prevNodes[prevNodes.size()-1];
            //_logFile << "\t\tVisit: " << BiGraph::nodeIndex_to_nodeName(nodeIndex) << endl;

            vector<u_int32_t> successors;
            getSuccessors(nodeIndex, 0, successors);

            if(successors.size() == 0) return;

            u_int64_t maxSharedReads = 0;
            u_int32_t maxSuccessorNodeIndex = -1;

            for(u_int32_t nodeIndex_successor : successors){

                vector<u_int32_t> nodeNames = prevNodeNames;
                //for(u_int32_t nodeName : anchorNodeNames){
                //    if(std::find(nodeNames.begin(), nodeNames.end(), nodeName) != nodeNames.end()) continue;
                //    nodeNames.push_back(nodeName);
                //}
                //if(std::find(nodeNames.begin(), nodeNames.end(), BiGraph::nodeIndex_to_nodeName(nodeIndex_successor)) == nodeNames.end()){
                nodeNames.push_back(BiGraph::nodeIndex_to_nodeName(nodeIndex_successor));
                //}

                const vector<u_int64_t>& readIndexes = unitigDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex_successor)]._readIndexes;
                //u_int32_t nbSharedReads = Utils::computeSharedReads(readIndexes_source, readIndexes);
                u_int32_t nbSharedReads = Utils::computeSharedReads(nodeNames, unitigDatas);
                
                //_logFile << "\t\tSuccessor: " << BiGraph::nodeIndex_to_nodeName(nodeIndex_successor) << " " << nbSharedReads << " " << (isVisited.find(nodeIndex_successor) != isVisited.end()) << endl;
                if(isVisited.find(nodeIndex_successor) != isVisited.end()) continue;



                if(nbSharedReads <= 0) continue;

                if(nbSharedReads > maxSharedReads){
                    maxSharedReads = nbSharedReads;
                    maxSuccessorNodeIndex = nodeIndex_successor;
                }
            }

            if(maxSuccessorNodeIndex == -1) return;

            isVisited.insert(maxSuccessorNodeIndex);
            prevNodes.push_back(maxSuccessorNodeIndex);
            prevNodeNames.push_back(BiGraph::nodeIndex_to_nodeName(maxSuccessorNodeIndex));
            if(BiGraph::nodeIndex_to_nodeName(maxSuccessorNodeIndex) == dest_nodeName) break; //Found path
        }     

        //_logFile << "\t\tfind path: ";
        //for(u_int32_t nodeIndex : prevNodes){
        //    _logFile << BiGraph::nodeIndex_to_nodeName(nodeIndex) << " ";
        //}
        //_logFile << endl;

        nodepath = prevNodes;    
    }

    void shortestPath_nodeName(u_int32_t source_nodeName, u_int32_t dest_nodeName, vector<u_int32_t>& path, bool includeSource, bool includeSink, u_int32_t maxDistance, const vector<UnitigData>& unitigDatas, unordered_set<u_int32_t>& unallowedNodenames, u_int32_t nodeIndex_sourceLala, vector<u_int32_t>& anchorNodeNames){


        
        //const vector<u_int64_t>& readIndexes_source = unitigDatas[anchorNodeName]._readIndexes;

        path.clear();
		unordered_set<u_int32_t> isVisited1;
		unordered_map<u_int32_t, u_int32_t> prev1;
        //queue<u_int32_t> queue1;
        priority_queue<NodeSupport, vector<NodeSupport> , NodeSupport_Comparator> queue1;
		unordered_set<u_int32_t> isVisited2;
		unordered_map<u_int32_t, u_int32_t> prev2;
        //queue<u_int32_t> queue2;
        priority_queue<NodeSupport, vector<NodeSupport> , NodeSupport_Comparator> queue2;
		//unordered_map<u_int32_t, u_int32_t> distance1;
		//unordered_map<u_int32_t, u_int32_t> distance2;

		unordered_map<u_int32_t, u_int32_t> prev;

        u_int32_t source_nodeIndex_1 = BiGraph::nodeName_to_nodeIndex(source_nodeName, true);
        u_int32_t source_nodeIndex_2 = BiGraph::nodeName_to_nodeIndex(source_nodeName, false);

        //unordered_map<u_int32_t, u_int32_t> dist1;
        //unordered_map<u_int32_t, u_int32_t> dist2;

        //distance1[source_nodeIndex_1] = 0;
        //distance1[source_nodeIndex_2] = 0;

        if(nodeIndex_sourceLala == -1){
            //isVisited1.insert(source_nodeIndex_1);
            prev1[source_nodeIndex_1] = -1;
            queue1.push({source_nodeIndex_1, 0});
            //isVisited2.insert(source_nodeIndex_2);
            prev2[source_nodeIndex_2] = -1;
            queue2.push({source_nodeIndex_2, 0});
        }
        else{
            //isVisited1.insert(nodeIndex_sourceLala);
            prev1[nodeIndex_sourceLala] = -1;
            queue1.push({nodeIndex_sourceLala, 0});
        }


        for(u_int32_t nodeName : unallowedNodenames){
            isVisited1.insert(BiGraph::nodeName_to_nodeIndex(nodeName, true));
            isVisited1.insert(BiGraph::nodeName_to_nodeIndex(nodeName, false));
            isVisited2.insert(BiGraph::nodeName_to_nodeIndex(nodeName, true));
            isVisited2.insert(BiGraph::nodeName_to_nodeIndex(nodeName, false));
        }
        //isVisited1.insert(BiGraph::nodeName_to_nodeIndex(unallowedNodename, true));
        //isVisited1.insert(BiGraph::nodeName_to_nodeIndex(unallowedNodename, false));
        //isVisited2.insert(BiGraph::nodeName_to_nodeIndex(unallowedNodename, true));
        //isVisited2.insert(BiGraph::nodeName_to_nodeIndex(unallowedNodename, false));

        bool found = false;

        u_int32_t source_nodeIndex = -1;
        u_int32_t dest_nodeIndex = -1;
        
        while(!found){

            //_logFile << source_nodeName << " " << dest_nodeName << endl;
            //_logFile << queue1.size() << " " << queue2.size() << endl;


            //_logFile << "\tTop1: " << BiGraph::nodeIndex_to_nodeName(queue2.top()._nodeIndex) << " " << queue2.top()._nbSupportingReads << endl;
            
            if(nodeIndex_sourceLala == -1){


                if(queue1.empty() && queue2.empty()) break;

                if(!queue1.empty() && !found){

                    NodeSupport nodeSupport = queue1.top();
                    queue1.pop();
                    u_int32_t nodeIndex_current = nodeSupport._nodeIndex;

                    //if (isVisited1.find(nodeIndex_current) != isVisited1.end()) continue;
                    //isVisited1.insert(nodeIndex_current);

                    //_logFile << "VisitL: " << BiGraph::nodeIndex_to_nodeName(nodeIndex_current) << endl;

                    vector<u_int32_t> successors;
                    getSuccessors(nodeIndex_current, 0, successors);

                    for(u_int32_t nodeIndex_successor : successors){

                        if (isVisited1.find(nodeIndex_successor) != isVisited1.end()) continue;
                    
                        
                        vector<u_int32_t> nodeNames = anchorNodeNames;
                        //vector<u_int32_t> nodeNames = prevNodeNames;
                        //for(u_int32_t nodeName : anchorNodeNames){
                        //    if(std::find(nodeNames.begin(), nodeNames.end(), nodeName) != nodeNames.end()) continue;
                        //    nodeNames.push_back(nodeName);
                        //}
                        //if(std::find(nodeNames.begin(), nodeNames.end(), BiGraph::nodeIndex_to_nodeName(nodeIndex_successor)) == nodeNames.end()){
                        nodeNames.push_back(BiGraph::nodeIndex_to_nodeName(nodeIndex_successor));
                        //}

                        //ifconst vector<u_int64_t>& readIndexes = unitigDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex_successor)]._readIndexes;
                        //u_int32_t nbSharedReads = Utils::computeSharedReads(readIndexes_source, readIndexes);
                        u_int32_t nbSharedReads = Utils::computeSharedReads(nodeNames, unitigDatas);

                        


                        //const vector<u_int64_t>& readIndexes = unitigDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex_successor)]._readIndexes;
                        //u_int32_t nbSharedReads = Utils::computeSharedReads(readIndexes_source, readIndexes);
                        //_logFile << "\tNeighborsL: " << BiGraph::nodeIndex_to_nodeName(nodeIndex_successor) << " " << nbSharedReads << endl;

                        if(nbSharedReads <= 1) continue;

                        //if(dist1.find(nodeIndex_successor) == dist1.end()){
                        //    dist1[nodeIndex_successor] = nbSharedReads;
                        //}

                        //if(dist1[nodeIndex_successor] > dist1[nodeIndex_current]){
                            

                            //dist1[nodeIndex_successor] = nbSharedReads; //dist1[nodeIndex_current] + ;
                            queue1.push({nodeIndex_successor, nbSharedReads});

                            prev1[nodeIndex_successor] = nodeIndex_current;

                            //distance1[nodeIndex_successor] = distance1[nodeIndex_current] + 1;
                            //if(distance1[nodeIndex_successor] > maxDistance) continue;

                            //queue1.push({nodeIndex_successor, nbSharedReads});
                            isVisited1.insert(nodeIndex_successor);
                            //prev1[nodeIndex_successor] = nodeIndex_current;
                            
                            if(BiGraph::nodeIndex_to_nodeName(nodeIndex_successor) == dest_nodeName){
                                source_nodeIndex = source_nodeIndex_1;
                                dest_nodeIndex = nodeIndex_successor;
                                found = true;
                                prev = prev1;
                                //_logFile << "found 1" << endl;
                                break;
                            }
                        //}

                        if(found) break;
                    }
                }

                //_logFile << "\tTop2: " << BiGraph::nodeIndex_to_nodeName(queue2.top()._nodeIndex) << " " << queue2.top()._nbSupportingReads << endl;
                
                if(!queue2.empty() && !found){

                    NodeSupport nodeSupport = queue2.top();
                    queue2.pop();
                    u_int32_t nodeIndex_current = nodeSupport._nodeIndex;

                    //if (isVisited2.find(nodeIndex_current) != isVisited2.end()) continue;
                    //isVisited2.insert(nodeIndex_current);
                    //_logFile << "VisitR: " << BiGraph::nodeIndex_to_nodeName(nodeIndex_current) << endl;

                    vector<u_int32_t> successors;
                    getSuccessors(nodeIndex_current, 0, successors);

                    for(u_int32_t nodeIndex_successor : successors){

                        if (isVisited2.find(nodeIndex_successor) != isVisited2.end()) continue;
                    
                        vector<u_int32_t> nodeNames = anchorNodeNames;
                        nodeNames.push_back(BiGraph::nodeIndex_to_nodeName(nodeIndex_successor));
                        u_int32_t nbSharedReads = Utils::computeSharedReads(nodeNames, unitigDatas);

                        //const vector<u_int64_t>& readIndexes = unitigDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex_successor)]._readIndexes;
                        //u_int32_t nbSharedReads = Utils::computeSharedReads(readIndexes_source, readIndexes);
                        //_logFile << "\tNeighborsR: " << BiGraph::nodeIndex_to_nodeName(nodeIndex_successor) << " " << nbSharedReads << endl;

                        if(nbSharedReads <= 1) continue;

                        //if(dist2.find(nodeIndex_successor) == dist2.end()){
                        //    dist2[nodeIndex_successor] = nbSharedReads;
                        //}

                        //if(dist2[nodeIndex_successor] > dist2[nodeIndex_current]){
                            

                        //dist2[nodeIndex_successor] = nbSharedReads; //dist2[nodeIndex_current] + nbSharedReads;
                        //_logFile << "\tInsert: " << BiGraph::nodeIndex_to_nodeName(nodeIndex_successor) << " " << nbSharedReads << endl;

                        queue2.push({nodeIndex_successor, nbSharedReads});
                        //_logFile << "\tTop: " << BiGraph::nodeIndex_to_nodeName(queue2.top()._nodeIndex) << " " << queue2.top()._nbSupportingReads << endl;
                        prev2[nodeIndex_successor] = nodeIndex_current;

                        //distance1[nodeIndex_successor] = distance1[nodeIndex_current] + 1;
                        //if(distance1[nodeIndex_successor] > maxDistance) continue;

                        //queue1.push({nodeIndex_successor, nbSharedReads});
                        isVisited2.insert(nodeIndex_successor);
                        //prev1[nodeIndex_successor] = nodeIndex_current;
                        
                        if(BiGraph::nodeIndex_to_nodeName(nodeIndex_successor) == dest_nodeName){
                            source_nodeIndex = source_nodeIndex_2;
                            dest_nodeIndex = nodeIndex_successor;
                            found = true;
                            prev = prev2;
                            //_logFile << "found 2" << endl;
                            break;
                        }
                        //}

                        if(found) break;
                    }

                    //_logFile << "\tTop: " << BiGraph::nodeIndex_to_nodeName(queue2.top()._nodeIndex) << " " << queue2.top()._nbSupportingReads << endl;
                            
                }
            }
            else{

                if(queue1.empty()) break;

                if(!queue1.empty() && !found){

                    NodeSupport nodeSupport = queue1.top();
                    queue1.pop();
                    u_int32_t nodeIndex_current = nodeSupport._nodeIndex;

                    //if (isVisited1.find(nodeIndex_current) != isVisited1.end()) continue;
                    //isVisited1.insert(nodeIndex_current);
                    
                    //_logFile << "Visit: " << BiGraph::nodeIndex_to_nodeName(nodeIndex_current) << endl;

                    vector<u_int32_t> successors;
                    getSuccessors(nodeIndex_current, 0, successors);

                    for(u_int32_t nodeIndex_successor : successors){

                        if (isVisited1.find(nodeIndex_successor) != isVisited1.end()) continue;
                    
                        vector<u_int32_t> nodeNames = anchorNodeNames;
                        nodeNames.push_back(BiGraph::nodeIndex_to_nodeName(nodeIndex_successor));
                        u_int32_t nbSharedReads = Utils::computeSharedReads(nodeNames, unitigDatas);


                        //const vector<u_int64_t>& readIndexes = unitigDatas[BiGraph::nodeIndex_to_nodeName(nodeIndex_successor)]._readIndexes;
                        //u_int32_t nbSharedReads = Utils::computeSharedReads(readIndexes_source, readIndexes);
                        //_logFile << "\tNeighbors: " << BiGraph::nodeIndex_to_nodeName(nodeIndex_successor) << " " << nbSharedReads << endl;

                        if(nbSharedReads <= 1) continue;

                        //if(dist1.find(nodeIndex_successor) == dist1.end()){
                        //    dist1[nodeIndex_successor] = nbSharedReads;
                        //}

                        //if(dist1[nodeIndex_successor] > dist1[nodeIndex_current]){
                            

                            //dist1[nodeIndex_successor] = nbSharedReads; //dist1[nodeIndex_current] + ;
                            queue1.push({nodeIndex_successor, nbSharedReads});

                            prev1[nodeIndex_successor] = nodeIndex_current;

                            //distance1[nodeIndex_successor] = distance1[nodeIndex_current] + 1;
                            //if(distance1[nodeIndex_successor] > maxDistance) continue;

                            //queue1.push({nodeIndex_successor, nbSharedReads});
                            isVisited1.insert(nodeIndex_successor);
                            //prev1[nodeIndex_successor] = nodeIndex_current;
                            
                            if(BiGraph::nodeIndex_to_nodeName(nodeIndex_successor) == dest_nodeName){
                                source_nodeIndex = nodeIndex_sourceLala;
                                dest_nodeIndex = nodeIndex_successor;
                                found = true;
                                prev = prev1;
                                //_logFile << "found 1" << endl;
                                break;
                            }
                        //}

                        if(found) break;
                    }
                }
            }
            
            //_logFile << "\tTop: " << BiGraph::nodeIndex_to_nodeName(queue2.top()._nodeIndex) << " " << queue2.top()._nbSupportingReads << endl;
                
         
            //if(source_nodeName == 6999 && dest_nodeName == 5632) getchar();
            //getchar(); 
        } 

        //_logFile << "\tFound: " << found << endl;

        if(found){

            u_int32_t n = dest_nodeIndex;
            while(n != source_nodeIndex){

                //_logFile << BiGraph::nodeIndex_to_nodeName(source_nodeIndex) << " " << BiGraph::nodeIndex_to_nodeName(dest_nodeIndex) << " " << BiGraph::nodeIndex_to_nodeName(n) << " " << n << endl;
                //_logFile << "haaaaa" << endl;

                if(n == dest_nodeIndex){
                    if(includeSink) path.push_back(n);
                }
                else{
                    path.push_back(n);
                }

                n = prev[n];
            }
            if(includeSource) path.push_back(source_nodeIndex);

            //return distance[dest_nodeIndex];
        }


        //return -1;

        
    }



    void findUniquePath(u_int32_t source_nodeIndex, u_int32_t dest_nodeIndex, vector<u_int32_t>& path, bool includeSource, bool includeSink, u_int32_t maxDistance){


        path.clear();

        if(nodeIndex_to_unitigIndex(source_nodeIndex) != nodeIndex_to_unitigIndex(dest_nodeIndex)) return;

		unordered_set<u_int32_t> isVisited;
		unordered_map<u_int32_t, u_int32_t> prev;
        queue<u_int32_t> queue;
		unordered_map<u_int32_t, u_int32_t> distance;

        prev[source_nodeIndex] = -1;
        queue.push(source_nodeIndex);

        bool found = false;
        
        while(!queue.empty() && !found){

            u_int32_t nodeIndex_current = queue.front();
            queue.pop();

            vector<u_int32_t> successors;
            getSuccessors(nodeIndex_current, 0, successors);

            if(successors.size() != 1) continue;

            u_int32_t nodeIndex_successor = successors[0];
        
            if (isVisited.find(nodeIndex_successor) != isVisited.end()) continue;
            
            distance[nodeIndex_successor] = distance[nodeIndex_current] + 1;
            if(distance[nodeIndex_successor] > maxDistance) continue;

            queue.push(nodeIndex_successor);
            prev[nodeIndex_successor] = nodeIndex_current;
            isVisited.insert(nodeIndex_successor);
        
                    
            if(nodeIndex_successor == dest_nodeIndex){
                found = true;
                break;
            }

        }


        if(found){

            u_int32_t n = dest_nodeIndex;
            while(n != source_nodeIndex){


                if(n == dest_nodeIndex){
                    if(includeSink) path.push_back(n);
                }
                else{
                    path.push_back(n);
                }

                n = prev[n];
            }
            if(includeSource) path.push_back(source_nodeIndex);

        }

        
    }
    */

/*
    void findUniquePath(u_int32_t source_nodeIndex, u_int32_t dest_nodeIndex, vector<u_int32_t>& path, bool includeSource, bool includeSink, u_int32_t maxDistance, unordered_set<u_int64_t>& readMinimizersIndex, bool print_read, unordered_map<u_int32_t, vector<u_int64_t>>& _nodeName_to_kminmerSequence){

        //print_read = true;
        //_logFile << BiGraph::nodeIndex_to_nodeName(source_nodeIndex) << " -> " << BiGraph::nodeIndex_to_nodeName(dest_nodeIndex) << endl;
        path.clear();
		//unordered_set<u_int32_t> isVisited;
		unordered_map<u_int32_t, u_int32_t> prev;
        //queue<u_int32_t> queue;
        priority_queue<NodeMissmatches, vector<NodeMissmatches> , NodeMissmatches_Comparator> queue;
		unordered_map<u_int32_t, u_int32_t> distance;
        unordered_map<u_int32_t, u_int32_t> nodesNbMissmatches;

        prev[source_nodeIndex] = -1;
        queue.push({source_nodeIndex, 0});
        u_int64_t minFoundNbmissmatches = -1;
        u_int64_t nbFound = 0;

        bool found = false;
        
        while(!queue.empty() && !found){

            NodeMissmatches nodeMissmatch = queue.top();
            u_int32_t nodeIndex_current = nodeMissmatch._nodeIndex;
            //u_int32_t nodeName_current = BiGraph::nodeIndex_to(nodeIndex_current);
            queue.pop();

            //if(found){
            //    if(nodeMissmatch._nbMissmatches > minFoundNbmissmatches){
            //        continue;
            //    }
            //}

            if(nodeIndex_current == dest_nodeIndex){
                found = true;
                minFoundNbmissmatches = nodeMissmatch._nbMissmatches;
                nbFound += 1;
                //continue;
                break;
            }

            if(distance[nodeIndex_current] > maxDistance) continue;

            vector<u_int32_t> successors;
            getSuccessors(nodeIndex_current, 0, successors);

            for(u_int32_t nodeIndex_successor : successors){

                u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(nodeIndex_successor);
                int nbMissmatches = 0;
                for(u_int64_t m : _nodeName_to_kminmerSequence[nodeName]){
                    if(readMinimizersIndex.find(m) == readMinimizersIndex.end()){
                        nbMissmatches += 1;
                    }
                }
                
                if(nodesNbMissmatches.find(nodeIndex_successor) == nodesNbMissmatches.end()){
                    nodesNbMissmatches[nodeIndex_successor] = -1;
                }

                if(nodesNbMissmatches[nodeIndex_successor] > nodesNbMissmatches[nodeIndex_current] + nbMissmatches){

                    nodesNbMissmatches[nodeIndex_successor] = nodesNbMissmatches[nodeIndex_current] + nbMissmatches;

                    distance[nodeIndex_successor] = distance[nodeIndex_current] + 1;

                    queue.push({nodeIndex_successor, nodesNbMissmatches[nodeIndex_successor]});
                    prev[nodeIndex_successor] = nodeIndex_current;
                    //isVisited.insert(nodeIndex_successor);

                }




                //if (isVisited.find(nodeIndex_successor) != isVisited.end()) continue;
                


            }


        }

        //if(nbFound > 1){
        //    _logFile << "haha" << endl;
        //    getchar();
        //}
        if(found){

            u_int32_t n = dest_nodeIndex;
            while(n != source_nodeIndex){


                if(n == dest_nodeIndex){
                    if(includeSink) path.push_back(n);
                }
                else{
                    path.push_back(n);
                }

                n = prev[n];
            }
            if(includeSource) path.push_back(source_nodeIndex);

        }

        
    }
*/
/*
	void detectRoundabouts(u_int64_t maxLength, const vector<UnitigData>& unitigDatas){

        #ifdef PRINT_DEBUG_SIMPLIFICATION
            _logFile << "detecting roudabout" << endl;
        #endif

        unordered_set<u_int32_t> isProcessedNodename;

        for(const Unitig& unitig: _unitigs){
            if(unitig._startNode == -1) continue;
            //for(u_int32_t nodeIndex : _isNodeValid2){
            //if(!isEdgeNode(nodeIndex)) continue;

            detectRoundabouts2(unitig._startNode, maxLength, unitigDatas, isProcessedNodename);
            detectRoundabouts2(unitig._endNode, maxLength, unitigDatas, isProcessedNodename);
    
        }

        #ifdef PRINT_DEBUG_SIMPLIFICATION
            _logFile << "done" << endl;
        #endif


	}

	void detectRoundabouts2(u_int32_t nodeIndex_source, u_int64_t maxLength, const vector<UnitigData>& unitigDatas, unordered_set<u_int32_t>& isProcessedNodename){
        
        u_int32_t source_nodeName = BiGraph::nodeIndex_to_nodeName(nodeIndex_source);
        
        if(isProcessedNodename.find(source_nodeName) != isProcessedNodename.end()) return;
        isProcessedNodename.insert(source_nodeName);

        //_logFile << "1" << endl;

        unordered_set<u_int32_t> readpathNodeNames;
        extractReadpathSubgraph(source_nodeName, unitigDatas, readpathNodeNames);

        //_logFile << "2" << endl;
        GraphSimplify* graph = new GraphSimplify(this, readpathNodeNames);
        
        //_logFile << "3" << endl;
        unordered_set<u_int32_t> bubbleNodeNames;
        graph->detectBubbles(this, readpathNodeNames, maxLength, unitigDatas, bubbleNodeNames);

        //_logFile << "4" << endl;
        for(u_int32_t nodeName : bubbleNodeNames){
            _isNodenameRoundabout.insert(nodeName);
        }

        //_logFile << "5" << endl;
        delete graph;
        //_logFile << "6" << endl;
        
    }

    void extractReadpathSubgraph(u_int32_t source_nodeName, const vector<UnitigData>& unitigDatas, unordered_set<u_int32_t>& component){

		component.clear();

        //_logFile << "-----" << endl;
        const UnitigData& source_readIndexes = unitigDatas[source_nodeName];

		//ofstream file_scc("/home/gats/workspace/run/subgraph.csv");
		//file_scc << "Name,Colour" << endl;

        unordered_set<u_int32_t> isVisited;
        queue<u_int32_t> queue;

        queue.push(source_nodeName);

        while (!queue.empty()){

            u_int64_t nodeName = queue.front();
            queue.pop();

            if (isVisited.find(nodeName) != isVisited.end()) continue;
            isVisited.insert(nodeName);


            const UnitigData& readIndexes = unitigDatas[nodeName];
            //_logFile << _unitigDatas2[unitigIndex]._readIndexes.size() << endl;

            u_int32_t nbSharedReads = Utils::computeSharedReads(source_readIndexes, readIndexes);
            //_logFile << nbSharedReads << endl;
            //_logFile << BiGraph::nodeIndex_to_nodeName(_unitigs[unitigIndex]._endNode) << " " << nbSharedReads << endl;
            //_logFile << "Shared reads: " << nbSharedReads << " " << readIndexes._readIndexes.size() << " " << source_readIndexes._readIndexes.size() << endl;
            if(nbSharedReads <= 1) continue;

            const Unitig& unitig = nodeIndex_to_unitig(BiGraph::nodeName_to_nodeIndex(nodeName, false));
            for(u_int32_t nodeIndex : unitig._nodes){
			    component.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex));
            }

            //for(u_int32_t nodeIndex : _unitigs[unitigIndex]._nodes){
            //    file_scc << BiGraph::nodeIndex_to_nodeName(nodeIndex) << "," << "red" << endl;
            //}

            vector<u_int32_t> neighbors;

            vector<u_int32_t> successors;
            getSuccessors(BiGraph::nodeName_to_nodeIndex(nodeName, false), 0, successors);
            
            for(u_int32_t nodeIndexSuccessor : successors){
                neighbors.push_back(nodeIndexSuccessor);
            }

            vector<u_int32_t> predecessors;
            getSuccessors(BiGraph::nodeName_to_nodeIndex(nodeName, true), 0, predecessors);
            
            for(u_int32_t nodeIndexSuccessor : predecessors){
                neighbors.push_back(nodeIndexSuccessor);
            }


            for(u_int32_t nodeIndexNeighbor : neighbors){
                const Unitig& unitig = nodeIndex_to_unitig(nodeIndexNeighbor);
                queue.push(BiGraph::nodeIndex_to_nodeName(unitig._startNode));
                queue.push(BiGraph::nodeIndex_to_nodeName(unitig._endNode));
                //for(u_int32_t nodeIndex : unitig._nodes){

			        //component.insert(BiGraph::nodeIndex_to_nodeName(nodeIndex));

                //    if(isEdgeNode(nodeIndex)){
                //        queue.push(BiGraph::nodeIndex_to_nodeName(nodeIndex));
                //    }
                //}
            }

        }

        //file_scc.close();

    }
    */

    struct UnitigOri{
        u_int32_t _unitigIndex;
        bool _ori;
    };

    unordered_set<u_int32_t> _debug_selectedUnitigIndex;

    void debug_selectUnitigIndex(){

		unordered_set<u_int32_t> writtenUnitigs;

        for(const Unitig& u: _unitigs){

			if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._startNode)) != writtenUnitigs.end()) continue;
			if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._endNode)) != writtenUnitigs.end()) continue;

			writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._startNode));
			writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._endNode));

            _debug_selectedUnitigIndex.insert(u._index);

        }
    }

    u_int32_t debug_nodeName_toSelectedUnitigIndex(u_int32_t nodeName){

        u_int32_t nodeIndex1 = BiGraph::nodeName_to_nodeIndex(nodeName, false);
        u_int32_t nodeIndex2 = BiGraph::nodeName_to_nodeIndex(nodeName, true);

        if(_isNodeRemoved[nodeIndex1] && _isNodeRemoved[nodeIndex2]) return -1;
        //if(_isNodeValid2.find(nodeIndex1) == _isNodeValid2.end() && _isNodeValid2.find(nodeIndex2) == _isNodeValid2.end()) return -1;

        u_int32_t unitigIndex1 = nodeIndex_to_unitigIndex(nodeIndex1);
        u_int32_t unitigIndex2 = nodeIndex_to_unitigIndex(nodeIndex2);
        u_int32_t unitigIndex = -1;

        if(_debug_selectedUnitigIndex.find(unitigIndex1) != _debug_selectedUnitigIndex.end()){
            unitigIndex = unitigIndex1;
        }
        else if(_debug_selectedUnitigIndex.find(unitigIndex2) != _debug_selectedUnitigIndex.end()){
            unitigIndex = unitigIndex2;
        }

        return unitigIndex; 
    }

    void saveUnitigGraph(const string& outputFilename, MDBG* mdbg, size_t minimizerSize, size_t nbCores, bool isCleanedGraph){

        _logFile << "Saving unitig graph: " << outputFilename << endl;
        
        nbCores = 1;
		unordered_set<u_int32_t> writtenUnitigs;

        unordered_set<u_int32_t> selectedUnitigIndex;

        ofstream file_nodeNameToUnitigIndex(_outputDir +  "/nodeName_to_unitigIndex.bin");
        //u_int32_t unitigIndex_source = -1;

        //_logFile << _unitigs.size() << endl;
        for(const Unitig& u: _unitigs){
            if(u._startNode == -1) continue;

			if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._startNode)) != writtenUnitigs.end()) continue;
			if(writtenUnitigs.find(BiGraph::nodeIndex_to_nodeName(u._endNode)) != writtenUnitigs.end()) continue;

			writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._startNode));
			writtenUnitigs.insert(BiGraph::nodeIndex_to_nodeName(u._endNode));

            //_logFile << "line" << endl;

            //if(_contigFeature == nullptr) outputFile << "S" << "\t" << u._index << "\t" << "*" << "\t" << "LN:i:600" << "\t" << 'dp:i:39' << endl;
            //_logFile << "Select: " << u._index << " " << u._nbNodes << endl;
            selectedUnitigIndex.insert(u._index);

            for(u_int32_t nodeIndex : u._nodes){
                u_int32_t nodeName = BiGraph::nodeIndex_to_nodeName(nodeIndex);
		        file_nodeNameToUnitigIndex.write((const char*)&nodeName, sizeof(nodeName));
		        file_nodeNameToUnitigIndex.write((const char*)&u._index, sizeof(u._index));
            }
            //if(unitigIndex_source == -1) unitigIndex_source = u._index;

            /*
            if(isCleanedGraph){
                if(u._index == 448){
                    vector<u_int32_t> succ;
                    getPredecessors(u._startNode, 0, succ);
                    _logFile << succ.size() << endl;
                    getSuccessors(u._endNode, 0, succ);
                    _logFile << succ.size() << endl;
                    _logFile << BiGraph::nodeIndex_to_nodeName(u._startNode) << " " << BiGraph::nodeIndex_to_nodeName(u._endNode) << endl;
                    getchar();
                }
            }
            */
            //mark[u._startNode] = i<<1 | 0;
            //mark[u._endNode] = i<<1 | 1;

            //i += 1;
        }

        file_nodeNameToUnitigIndex.close();

        //if(_contigFeature == nullptr){
            
            ofstream outputFile(outputFilename);

            //writeReadPath(mdbg, minimizerSize, nbCores, selectedUnitigIndex, isCleanedGraph);
            //if(_kminmerSize == 4 && doesWriteReadIndex) writeReadIndex(mdbg, minimizerSize, nbCores, selectedUnitigIndex);

            unordered_set<u_int32_t> linkedUnitigIndex;
            unordered_set<DbgEdge, hash_pair> isEdge;
            unordered_set<u_int32_t> isVisited;
            //_logFile << outputFilename << endl;
            //outputFile << "lala" << endl;
            

            for(const Unitig& unitig: _unitigs){

                if(selectedUnitigIndex.find(unitig._index) == selectedUnitigIndex.end()) continue;
                if(isVisited.find(unitig._index) != isVisited.end()) continue;

                queue<u_int32_t> queue;
                queue.push(unitig._index);

                while(queue.size() > 0){
                    u_int32_t unitigIndex = queue.front();
                    queue.pop();

                    if(isVisited.find(unitigIndex) != isVisited.end()) continue;
                    isVisited.insert(unitigIndex);

                    string ori = "+";
                    if(selectedUnitigIndex.find(unitigIndex) == selectedUnitigIndex.end()){
                        unitigIndex = unitigIndex_toReverseDirection(unitigIndex);
                        ori = "-";
                    }

                    //isVisited.insert(unitigIndex_toReverseDirection(unitigIndex));
                    //linkedUnitigIndex.insert(unitigIndex);

                    outputFile << "S" << "\t" << unitigIndex << "\t" << "*" << "\t" << "LN:i:" << _unitigs[unitigIndex]._length << "\t" << "dp:i:" << _unitigs[unitigIndex]._abundance << endl;


                    vector<u_int32_t> successors;
                    getSuccessors_unitig(unitigIndex, 0, successors);

                    for(u_int32_t unitigIndexN : successors){

                        string ori2 = "+";
                        if(selectedUnitigIndex.find(unitigIndexN) == selectedUnitigIndex.end()){
                            unitigIndexN = unitigIndex_toReverseDirection(unitigIndexN);
                            ori2 = "-";
                        }

                        //if(linkedUnitigIndex.find(unitigIndex_toReverseDirection(unitigIndexN)) != linkedUnitigIndex.end()){
                        //    unitigIndexNN = unitigIndex_toReverseDirection(unitigIndexN);
                        //    ori = "-";
                        //}
                        

                        //DbgEdge edge = {unitigIndex, unitigIndexN};
                        //edge = edge.normalize();
                        //if(isEdge.find(edge) != isEdge.end()) continue;
                        //isEdge.insert(edge);

                        //linkedUnitigIndex.insert(unitigIndexNN);
                        
                        u_int32_t overlap = 600;
                        outputFile << "L" << "\t" << unitigIndex << "\t" << ori << "\t" << unitigIndexN << "\t" << ori2 << "\t" << overlap << "M" << endl;
                        queue.push(unitigIndexN);
                    }

                    
                    vector<u_int32_t> predecessors;
                    getPredecessors_unitig(unitigIndex, 0, predecessors);
                    for(u_int32_t unitigIndexN : predecessors){

                        string ori2 = "+";
                        if(selectedUnitigIndex.find(unitigIndexN) == selectedUnitigIndex.end()){
                            unitigIndexN = unitigIndex_toReverseDirection(unitigIndexN);
                            ori2 = "-";
                        }

                        //if(linkedUnitigIndex.find(unitigIndex_toReverseDirection(unitigIndexN)) != linkedUnitigIndex.end()){
                        //    unitigIndexNN = unitigIndex_toReverseDirection(unitigIndexN);
                        //    ori = "-";
                        //}
                        

                        //DbgEdge edge = {unitigIndex, unitigIndexN};
                        //edge = edge.normalize();
                        //if(isEdge.find(edge) != isEdge.end()) continue;
                        //isEdge.insert(edge);

                        //linkedUnitigIndex.insert(unitigIndexNN);
                        
                        u_int32_t overlap = 600;
                        outputFile << "L" << "\t" << unitigIndexN << "\t" << ori2 << "\t" << unitigIndex << "\t" << ori << "\t" << overlap << "M" << endl;
                        queue.push(unitigIndexN);
                    }
                    

                    //if(unitigIndex == 452){
                    //    _logFile << successors.size() << " " << predecessors.size() << endl;
                    //}

                }
            }



            outputFile.close();
        //}
        //else{
        /*
        if(_contigFeature != nullptr){
            if(isCleanedGraph){

                unordered_map<u_int32_t, unordered_set<u_int32_t>> unitigIndex_to_contigIndex;

                for(const auto& it : _contigFeature->_nodeName_to_contigIndex){
                    u_int32_t nodeName = it.first;
                    const vector<u_int32_t>& contigIndexes = it.second;

                    u_int32_t unitigIndex = nodeName_toSelectedUnitigIndex(nodeName, selectedUnitigIndex);
                    if(unitigIndex == -1) continue;

                    for(u_int32_t contigIndex : contigIndexes){
                        unitigIndex_to_contigIndex[unitigIndex].insert(contigIndex);
                    }
                }

                unordered_set<u_int32_t> coloredUnitigs;
                ofstream outputFileContigColor(_outputDir + "/" + "contig_nt_color.csv");
                outputFileContigColor << "Name,Color" << endl;

                for(const auto& it : unitigIndex_to_contigIndex){
                    u_int32_t unitigIndex = it.first;
                    const unordered_set<u_int32_t>& contigIndexes = it.second;

                    if(contigIndexes.size() > 1){
                        outputFileContigColor << unitigIndex << "," << "red" << endl;
                        coloredUnitigs.insert(unitigIndex);
                    }
                    else{
                        for(u_int32_t contigIndex : contigIndexes){
                            outputFileContigColor << unitigIndex << "," << "green" << endl; //contigIndex
                            coloredUnitigs.insert(unitigIndex);
                        }
                    }

                }


                

                outputFileContigColor.close();
                //getchar();



                ofstream outputFileContigPath(_outputDir + "/" + "contig_path.txt");

                for(const auto& it : _contigFeature->_contigIndex_to_nodeName){
                    u_int32_t contigIndex = it.first;
                    const vector<u_int32_t>& nodeNames = it.second;

                    outputFileContigPath << "ctg" << contigIndex << ":";
                    u_int32_t prevUnitigIndex = -1;

                    for(size_t i=0; i<nodeNames.size(); i++){

                        u_int32_t nodeName = nodeNames[i];
                        u_int32_t unitigIndex = nodeName_toSelectedUnitigIndex(nodeName, selectedUnitigIndex);
                        if(unitigIndex == -1) continue;

                        if(unitigIndex != prevUnitigIndex){
                            if(i == 0){
                                outputFileContigPath << unitigIndex;
                            }
                            else{
                                outputFileContigPath << ";" << unitigIndex;
                            }
                            prevUnitigIndex = unitigIndex;
                        }

                    }

                    outputFileContigPath << endl;
                }

                outputFileContigPath.close();


                //_logFile << "lala" << endl;
                //getchar();
            }
        }
        */

        _logFile << "done" << endl;
        //getchar();

    }

	ofstream _file_readPath;



	void writeReadPath(MDBG* mdbg, size_t minimizerSize, size_t nbCores, unordered_set<u_int32_t>& selectedUnitigIndex, bool isCleanedGraph){

        if(isCleanedGraph){
		    _file_readPath = ofstream(_outputDir + "/read_path_cleaned.txt");
        }
        else{
		    _file_readPath = ofstream(_outputDir + "/read_path.txt");
        }

		KminmerParserParallel readParser(_outputDir + "/read_data.txt", minimizerSize, _kminmerSize, false, true, nbCores);
		readParser.parse(ReadPathFunctor(this, mdbg, selectedUnitigIndex));

		_file_readPath.close();

	}

	void writeReadPath(const vector<u_int32_t>& nodeNames){
		
		if(nodeNames.size() == 0) return;

		#pragma omp critical
		{

			_file_readPath << nodeNames[0];
			for(size_t i=1; i<nodeNames.size(); i++){
				_file_readPath << ";" << nodeNames[i];
			}
			_file_readPath << endl;
		
		}
	}

	class ReadPathFunctor {

		public:

		MDBG* _mdbg;
		GraphSimplify* _graph;
        unordered_set<u_int32_t>& _selectedUnitigIndex;

		ReadPathFunctor(GraphSimplify* graph, MDBG* mdbg, unordered_set<u_int32_t>& selectedUnitigIndex) : _selectedUnitigIndex(selectedUnitigIndex){
			_mdbg = mdbg;
			_graph = graph;
		}

		ReadPathFunctor(const ReadPathFunctor& copy): _selectedUnitigIndex(copy._selectedUnitigIndex){
			_mdbg = copy._mdbg;
			_graph = copy._graph;
		}


		void operator () (const KminmerList& kminmerList) {
		
			u_int64_t readIndex = kminmerList._readIndex;

			const vector<u_int64_t>& minimizers = kminmerList._readMinimizers;
			const vector<ReadKminmerComplete>& kminmersInfos = kminmerList._kminmersInfo;

			vector<u_int32_t> nodeNames;
            u_int32_t prevUnitigIndex = -1;

			for(size_t i=0; i<kminmersInfos.size(); i++){

				const ReadKminmerComplete& info = kminmersInfos[i];
				const KmerVec& vec = info._vec;

				if(_mdbg->_dbg_nodes.find(vec) == _mdbg->_dbg_nodes.end()){
					continue;
				}
				
				u_int32_t nodeName = _mdbg->_dbg_nodes[vec]._index;
                u_int32_t unitigIndex = _graph->nodeName_toSelectedUnitigIndex(nodeName, _selectedUnitigIndex);

                if(unitigIndex != prevUnitigIndex){
				    nodeNames.push_back(unitigIndex);
                    prevUnitigIndex = unitigIndex;
                }

			}

			_graph->writeReadPath(nodeNames);
		}
	};

	unordered_map<u_int32_t, unordered_set<u_int64_t>> _unitigIndex_to_readIndexes;
    MDBG* _mdbg;

    /*
	void writeReadIndex(MDBG* mdbg, size_t minimizerSize, size_t nbCores, unordered_set<u_int32_t>& selectedUnitigIndex){

		ofstream file(_outputDir + "/read_index.txt");

		KminmerParserParallel readParser(_outputDir + "/read_data.txt", minimizerSize, _kminmerSize, false, nbCores);
		readParser.parse(ReadIndexFunctor(this, mdbg, selectedUnitigIndex));


		for(const auto& it: _unitigIndex_to_readIndexes){

			u_int32_t nodeName = it.first;
			string line = to_string(nodeName) + ":";

            size_t i=0;

            //_logFile << it.first << " " << it.second.size() << endl;

			for(u_int64_t readIndex : it.second){
				if(i==0){
					line += to_string(readIndex);
				}
				else{
					line += ";" + to_string(readIndex);
				}
                i += 1;
			}

			file << line << endl;
		}

		file.close();

        _unitigIndex_to_readIndexes.clear();
        _mdbg = nullptr;

        //_logFile << _unitigs[8372]._abundance << " " << _unitigs[8372]._nbNodes << endl;
	}
    */

    u_int32_t nodeName_toSelectedUnitigIndex(u_int32_t nodeName, unordered_set<u_int32_t>& selectedUnitigIndex){

        u_int32_t nodeIndex1 = BiGraph::nodeName_to_nodeIndex(nodeName, false);
        u_int32_t nodeIndex2 = BiGraph::nodeName_to_nodeIndex(nodeName, true);

        if(_isNodeRemoved[nodeIndex1] && _isNodeRemoved[nodeIndex2]) return -1;
        //if(_isNodeValid2.find(nodeIndex1) == _isNodeValid2.end() && _isNodeValid2.find(nodeIndex2) == _isNodeValid2.end()) return -1;

        u_int32_t unitigIndex1 = nodeIndex_to_unitigIndex(nodeIndex1);
        u_int32_t unitigIndex2 = nodeIndex_to_unitigIndex(nodeIndex2);
        u_int32_t unitigIndex = -1;

        if(selectedUnitigIndex.find(unitigIndex1) != selectedUnitigIndex.end()){
            unitigIndex = unitigIndex1;
        }
        else if(selectedUnitigIndex.find(unitigIndex2) != selectedUnitigIndex.end()){
            unitigIndex = unitigIndex2;
        }

        return unitigIndex; 
    }

    

};


#endif
