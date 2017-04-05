#ifndef LLVM_TRANSFORMS_UTILS_METADATAEXTRACTOR_H
#define LLVM_TRANSFORMS_UTILS_METADATAEXTRACTOR_H

#include <string>
#include <vector>
#include "llvm/Pass.h"
#include "llvm/IR/Type.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"

using namespace llvm;

namespace liquid {

	class RefinementMetadataForVariable
	{
	public:
		std::string OriginalName;
		std::string LLVMName;
		std::string OriginalType;
		llvm::Type* LLVMType;
		std::string Assume;
		std::string Verify;
	};

	class RefinementMetadata {
	public:
		std::vector<std::string> Qualifiers;
		std::vector<RefinementMetadataForVariable> Parameters;
		RefinementMetadataForVariable Return;

		static ResultType Extract(Function& F, RefinementMetadata& ret);
	};
}

#endif