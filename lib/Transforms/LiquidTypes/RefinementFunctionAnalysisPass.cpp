#include "llvm/Transforms/LiquidTypes/RefinementFunctionAnalysisPass.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Transforms/LiquidTypes/AnalysisRetriever.h"

using namespace liquid;

namespace llvm {

	namespace {

		void runRefinementAnalysis(Function &F, const DominatorTree& dominatorTree, const llvm::LoopInfo& loopInfo, const AnalysisRetriever& analysisRetriever, RefinementFunctionInfo& r)
		{
			auto metadata = F.getMetadata("refinement");
			//no refinement data
			if (metadata == nullptr)
			{
				return;
			}

			r.RefinementDataFound = true;

			{
				ResultType getRefData = RefinementMetadata_Raw::Extract(F, r.FnRefinementMetadata_Raw);
				if (!getRefData.Succeeded) { report_fatal_error(getRefData.ErrorMsg); }
			}

			{
				ResultType getRefData = RefinementMetadata::ParseMetadata(r.FnRefinementMetadata_Raw, r.ParsedFnRefinementMetadata);
				if (!getRefData.Succeeded) { report_fatal_error(getRefData.ErrorMsg); }
			}

			r.ConstraintGenerator = std::make_unique<RefinementConstraintGenerator>(F, dominatorTree);

			{
				ResultType constraintRes = r.ConstraintGenerator->BuildConstraintsFromSignature(r.ParsedFnRefinementMetadata);
				if (!constraintRes.Succeeded) { report_fatal_error(constraintRes.ErrorMsg); }
			}

			{
				ResultType constraintRes = r.ConstraintGenerator->BuildConstraintsFromInstructions(r.ParsedFnRefinementMetadata, analysisRetriever);
				if (!constraintRes.Succeeded) { report_fatal_error(constraintRes.ErrorMsg); }
			}

			{
				ResultType constraintRes = r.ConstraintGenerator->CaptureLoopConstraints(loopInfo);
				if (!constraintRes.Succeeded) { report_fatal_error(constraintRes.ErrorMsg); }
			}
		}
	}

	char RefinementFunctionAnalysisPass::ID = 0;
	INITIALIZE_PASS_BEGIN(RefinementFunctionAnalysisPass, "refinementAnalysis", "Refinement constraints Construction", true, true)
	INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
	INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
	INITIALIZE_PASS_END(RefinementFunctionAnalysisPass, "refinementAnalysis", "Refinement constraints Construction", true, true)

	bool RefinementFunctionAnalysisPass::runOnFunction(Function &F) {
		auto& dominatorTree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
		auto& loopInfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
		std::string key = F.getName().str();
		RI[key] = RefinementFunctionInfo();

		AnalysisRetriever analysisRetriever(&RI);
		runRefinementAnalysis(F, dominatorTree, loopInfo, analysisRetriever, RI[key]);

		return false;
	}

	void RefinementFunctionAnalysisPass::getAnalysisUsage(AnalysisUsage &AU) const {
		AU.addRequired<DominatorTreeWrapperPass>();
		AU.addRequired<LoopInfoWrapperPass>();
		AU.setPreservesAll();
	}

	AnalysisKey RefinementFunctionAnalysis::Key;
	RefinementFunctionInfo RefinementFunctionAnalysis::run(Function &F, FunctionAnalysisManager &AM)
	{
		RefinementFunctionInfo r;
		auto& dominatorTree = AM.getResult<DominatorTreeAnalysis>(F);
		auto& loopInfo = AM.getResult<LoopAnalysis>(F);

		AnalysisRetriever analysisRetriever(&AM, &F, &r);
		runRefinementAnalysis(F, dominatorTree, loopInfo, analysisRetriever, r);

		return r;
	}
}