#pragma once

#include <FiniteDifferenceSolver2D.h>

#define MAKE_DEFAULT_CONSTRUCTORS(CLASS)\
	virtual ~CLASS() noexcept = default;\
	CLASS(const CLASS& rhs) noexcept = default;\
	CLASS(CLASS&& rhs) noexcept = default;\
	CLASS& operator=(const CLASS& rhs) noexcept = default;\
	CLASS& operator=(CLASS&& rhs) noexcept = default;

namespace pde
{
	template<MemorySpace memorySpace = MemorySpace::Device, MathDomain mathDomain = MathDomain::Float>
	class WaveEquationSolver2D : public FiniteDifferenceSolver2D<WaveEquationSolver2D<memorySpace, mathDomain>, memorySpace, mathDomain>
	{
	public:
		// befriend the grandparent CRTP class
		friend class FiniteDifferenceSolver<WaveEquationSolver2D<memorySpace, mathDomain>, PdeInputData2D<memorySpace, mathDomain>, memorySpace, mathDomain>;
		// befriend the mother CRTP class
		friend class FiniteDifferenceSolver2D<WaveEquationSolver2D<memorySpace, mathDomain>, memorySpace, mathDomain>;

		using FiniteDifferenceSolver2D::FiniteDifferenceSolver2D;

		MAKE_DEFAULT_CONSTRUCTORS(WaveEquationSolver2D);

	protected:
		void AdvanceImpl(cl::ColumnWiseMatrix<memorySpace, mathDomain>& solution,
						 const std::shared_ptr<cl::Tensor<memorySpace, mathDomain>>& timeDiscretizers,
						 const SolverType solverType,
						 const unsigned nSteps = 1);

		void MakeTimeDiscretizer(const std::shared_ptr<cl::Tensor<memorySpace, mathDomain>>& timeDiscretizers, const SolverType solverType);

		void Setup(const unsigned solverSteps);
	};

#pragma region Type aliases

	typedef WaveEquationSolver2D<MemorySpace::Device, MathDomain::Float> GpuSingleWaveEquationSolver2D;
	typedef GpuSingleWaveEquationSolver2D GpuFloatWaveEquationSolver2D;
	typedef WaveEquationSolver2D<MemorySpace::Device, MathDomain::Double> GpuDoubleWaveEquationSolver2D;
	typedef WaveEquationSolver2D<MemorySpace::Host, MathDomain::Float> CpuSingleWaveEquationSolver2D;
	typedef CpuSingleWaveEquationSolver2D CpuFloatWaveEquationSolver2D;
	typedef WaveEquationSolver2D<MemorySpace::Host, MathDomain::Double> CpuDoubleSolver2D;
	typedef GpuSingleWaveEquationSolver2D wave2D;
	typedef GpuDoubleWaveEquationSolver2D dwave2D;

#pragma endregion
}

#undef MAKE_DEFAULT_CONSTRUCTORS

#include <WaveEquationSolver2D.tpp>
