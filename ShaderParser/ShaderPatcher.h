// Copyright 2015 XLGAMES Inc.
//
// Distributed under the MIT License (See
// accompanying file "LICENSE" or the website
// http://www.opensource.org/licenses/mit-license.php)

#pragma once

#include "../Assets/AssetUtils.h"
#include "../Utility/IteratorUtils.h"
#include "../Core/Types.h"
#include <string>
#include <vector>

namespace ShaderSourceParser { class FunctionSignature; }

namespace ShaderPatcher 
{

        ///////////////////////////////////////////////////////////////

    class Node
    {
    public:
        struct Type
        {
            enum Enum
            {
                Procedure,
                SlotInput,
                SlotOutput,
				Uniforms
            };
        };
        
        Node(const std::string& archiveName, uint32 nodeId, Type::Enum type);

		#if defined(COMPILER_DEFAULT_IMPLICIT_OPERATORS)
			Node(Node&& moveFrom) never_throws = default;
			Node& operator=(Node&& moveFrom) never_throws = default;
			Node(const Node& cloneFrom) = default;
			Node& operator=(const Node& cloneFrom) = default;
		#endif

        const std::string&  ArchiveName() const         { return _archiveName; }
        uint32              NodeId() const              { return _nodeId; }
        Type::Enum          GetType() const             { return _type; }
        
    private:
        std::string     _archiveName;
        uint32          _nodeId;
        Type::Enum      _type;
    };

        ///////////////////////////////////////////////////////////////

    class Type
    {
    public:
        std::string _name;
        Type() {}
        Type(const std::string& name) : _name(name) {}
    };

        ///////////////////////////////////////////////////////////////

    class NodeBaseConnection
    {
    public:
        NodeBaseConnection(uint32 outputNodeId, const std::string& outputParameterName);

        NodeBaseConnection(NodeBaseConnection&& moveFrom) never_throws;
        NodeBaseConnection& operator=(NodeBaseConnection&& moveFrom) never_throws;

		#if defined(COMPILER_DEFAULT_IMPLICIT_OPERATORS)
			NodeBaseConnection(const NodeBaseConnection&) = default;
			NodeBaseConnection& operator=(const NodeBaseConnection&) = default;
		#endif

        uint32      OutputNodeId() const                { return _outputNodeId; }
        const std::string&  OutputParameterName() const { return _outputParameterName; }

    protected:
        uint32          _outputNodeId;
        std::string     _outputParameterName;
    };

        ///////////////////////////////////////////////////////////////

    class NodeConnection : public NodeBaseConnection
    {
    public:
        NodeConnection( uint32 outputNodeId, uint32 inputNodeId, 
                        const std::string& outputParameterName,
                        const std::string& inputParameterName, const Type& inputType);

        NodeConnection(NodeConnection&& moveFrom) never_throws;
        NodeConnection& operator=(NodeConnection&& moveFrom) never_throws;

		#if defined(COMPILER_DEFAULT_IMPLICIT_OPERATORS)
			NodeConnection(const NodeConnection&) = default;
			NodeConnection& operator=(const NodeConnection&) = default;
		#endif

        uint32              InputNodeId() const         { return _inputNodeId; }
        const Type&         InputType() const           { return _inputType; }
        const std::string&  InputParameterName() const  { return _inputParameterName; }

    private:
        uint32          _inputNodeId;
        std::string     _inputParameterName;
        Type            _inputType;
    };

        ///////////////////////////////////////////////////////////////

    class ConstantConnection : public NodeBaseConnection
    {
    public:
        ConstantConnection(uint32 outputNodeId, const std::string& outputParameterName, const std::string& value);
        ConstantConnection(ConstantConnection&& moveFrom) never_throws;
        ConstantConnection& operator=(ConstantConnection&& moveFrom) never_throws;

        #if defined(COMPILER_DEFAULT_IMPLICIT_OPERATORS)
			ConstantConnection(const ConstantConnection&) = default;
			ConstantConnection& operator=(const ConstantConnection&) = default;
		#endif

        const std::string&  Value() const               { return _value; }

    private:
        std::string     _value;
    };

            ///////////////////////////////////////////////////////////////

    class InputParameterConnection : public NodeBaseConnection
    {
    public:
        InputParameterConnection(uint32 outputNodeId, const std::string& outputParameterName, const Type& type, const std::string& name, const std::string& semantic, const std::string& defaultValue);
        InputParameterConnection(InputParameterConnection&& moveFrom) never_throws;
        InputParameterConnection& operator=(InputParameterConnection&& moveFrom) never_throws;

        #if defined(COMPILER_DEFAULT_IMPLICIT_OPERATORS)
			InputParameterConnection(const InputParameterConnection&) = default;
			InputParameterConnection& operator=(const InputParameterConnection&) = default;
		#endif
        
        const Type&         InputType() const           { return _type; }
        const std::string&  InputName() const           { return _name; }
        const std::string&  InputSemantic() const       { return _semantic; }
        const std::string&  Default() const             { return _default; }

    private:
        Type            _type;
        std::string     _name;
        std::string     _semantic;
        std::string     _default;
    };

        ///////////////////////////////////////////////////////////////

    class NodeGraph
    {
    public:
        IteratorRange<const Node*>                      GetNodes() const                        { return MakeIteratorRange(_nodes); }
        IteratorRange<const NodeConnection*>            GetNodeConnections() const              { return MakeIteratorRange(_nodeConnections); }
        IteratorRange<const ConstantConnection*>        GetConstantConnections() const          { return MakeIteratorRange(_constantConnections); }
        IteratorRange<const InputParameterConnection*>  GetInputParameterConnections() const    { return MakeIteratorRange(_inputParameterConnections); }

        void Add(Node&&);
        void Add(NodeConnection&&);
        void Add(ConstantConnection&&);
        void Add(InputParameterConnection&&);

        std::string     GetName() const                 { return _name; }
        void            SetName(std::string newName)    { _name = newName; }
		void			SetSearchRules(const ::Assets::DirectorySearchRules& rules) { _searchRules = rules; }
		const ::Assets::DirectorySearchRules& GetSearchRules() const { return _searchRules; }

		void			Trim(const uint32* trimNodesBegin, const uint32* trimNodesEnd);
        void            TrimForPreview(uint32 previewNode);
        bool            TrimForOutputs(const std::string outputs[], size_t outputCount);
        void            AddDefaultOutputs();

        const Node*     GetNode(uint32 nodeId) const;

        explicit NodeGraph(const std::string& name = std::string());
        ~NodeGraph();

		#if defined(COMPILER_DEFAULT_IMPLICIT_OPERATORS)
			NodeGraph(NodeGraph&&) never_throws = default;
			NodeGraph& operator=(NodeGraph&&) never_throws = default;
			NodeGraph(const NodeGraph&) = default;
			NodeGraph& operator=(const NodeGraph&) = default;
		#endif

    private:
        std::vector<Node> _nodes;
        std::vector<NodeConnection> _nodeConnections;
        std::vector<ConstantConnection> _constantConnections;
        std::vector<InputParameterConnection> _inputParameterConnections;
        std::string _name;

		::Assets::DirectorySearchRules _searchRules;

        bool        IsUpstream(uint32 startNode, uint32 searchingForNode);
        bool        IsDownstream(uint32 startNode, const uint32* searchingForNodesStart, const uint32* searchingForNodesEnd);
        bool        HasNode(uint32 nodeId);
        uint32      GetUniqueNodeId() const;
        void        AddDefaultOutputs(const Node& node);
    };

        ///////////////////////////////////////////////////////////////

    class FunctionInterface
    {
    public:
		class Parameter
		{
		public:
			enum Direction { In, Out };
			std::string _type, _name, _archiveName, _semantic, _default;
			Direction _direction;
			Parameter(
				const std::string& type, const std::string& name, 
				const std::string& archiveName, 
				Direction direction = In,
				const std::string& semantic = std::string(), const std::string& defaultValue = std::string())
				: _type(type), _name(name), _archiveName(archiveName), _direction(direction), _semantic(semantic), _default(defaultValue) {}
			Parameter() {}
		};

        auto GetFunctionParameters() const -> IteratorRange<const Parameter*>	{ return MakeIteratorRange(_functionParameters); }
        auto GetGlobalParameters() const -> IteratorRange<const Parameter*>		{ return MakeIteratorRange(_globalParameters); }
		const std::string& GetName() const { return _name; }
        bool IsCBufferGlobal(unsigned c) const;

		void AddFunctionParameter(const Parameter& param);
		void AddGlobalParameter(const Parameter& param);
		void SetName(const std::string& newName) { _name = newName; }

        FunctionInterface();
        ~FunctionInterface();
    private:
        std::vector<Parameter> _functionParameters;
        std::vector<Parameter> _globalParameters;
		std::string _name;
    };

    std::string GenerateShaderHeader(const NodeGraph& graph);
    std::pair<std::string, FunctionInterface> GenerateFunction(const NodeGraph& graph);

    struct PreviewOptions
    {
    public:
        enum class Type { Object, Chart };
        Type _type;
        std::string _outputToVisualize;
		using VariableRestrictions = std::vector<std::pair<std::string, std::string>>;
        VariableRestrictions _variableRestrictions;
    };

    std::string GenerateStructureForPreview(
        StringSection<char> graphName, 
        const FunctionInterface& interf, 
		const ::Assets::DirectorySearchRules& searchRules,
        const PreviewOptions& previewOptions = { PreviewOptions::Type::Object, std::string(), PreviewOptions::VariableRestrictions() });

	std::string GenerateStructureForTechniqueConfig(const FunctionInterface& interf, const char graphName[]);

	std::string GenerateScaffoldFunction(const ShaderSourceParser::FunctionSignature& slotSignature, const FunctionInterface& generatedFunctionSignature);
}

