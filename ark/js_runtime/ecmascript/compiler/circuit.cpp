/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ecmascript/compiler/circuit.h"

namespace kungfu {
Circuit::Circuit() : space({}), circuitSize(0), gateCounter(0), time(1), dataSection({})
{
    this->NewGate(OpCode(OpCode::CIRCUIT_ROOT), 0, {}, TypeCode::NOTYPE);  // circuit root
    auto circuitRoot = Circuit::GetCircuitRoot(OpCode(OpCode::CIRCUIT_ROOT));
    this->NewGate(OpCode(OpCode::STATE_ENTRY), 0, {circuitRoot}, TypeCode::NOTYPE);
    this->NewGate(OpCode(OpCode::DEPEND_ENTRY), 0, {circuitRoot}, TypeCode::NOTYPE);
    this->NewGate(OpCode(OpCode::FRAMESTATE_ENTRY), 0, {circuitRoot}, TypeCode::NOTYPE);
    this->NewGate(OpCode(OpCode::RETURN_LIST), 0, {circuitRoot}, TypeCode::NOTYPE);
    this->NewGate(OpCode(OpCode::THROW_LIST), 0, {circuitRoot}, TypeCode::NOTYPE);
    this->NewGate(OpCode(OpCode::CONSTANT_LIST), 0, {circuitRoot}, TypeCode::NOTYPE);
    this->NewGate(OpCode(OpCode::ALLOCA_LIST), 0, {circuitRoot}, TypeCode::NOTYPE);
    this->NewGate(OpCode(OpCode::ARG_LIST), 0, {circuitRoot}, TypeCode::NOTYPE);
}

uint8_t *Circuit::AllocateSpace(size_t gateSize)
{
    this->circuitSize += gateSize;
    if (UNLIKELY(this->GetSpaceDataSize() == 0)) {
        this->SetSpaceDataSize(INITIAL_SPACE);
    }
    while (UNLIKELY(this->GetSpaceDataSize() < this->circuitSize)) {
        this->SetSpaceDataSize(this->GetSpaceDataSize() * SCALE_RATE);
    }
    if (UNLIKELY(this->GetSpaceDataSize() > MAX_SPACE)) {
        return nullptr;  // abort compilation
    }
    if (UNLIKELY(this->GetSpaceDataStartPtrConst() == nullptr)) {
        return nullptr;  // abort compilation
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return this->GetDataPtr(this->circuitSize - gateSize);
}

Gate *Circuit::AllocateGateSpace(size_t numIns)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return reinterpret_cast<Gate *>(this->AllocateSpace(Gate::GetGateSize(numIns)) + Gate::GetOutListSize(numIns));
}

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
AddrShift Circuit::NewGate(
    OpCode opcode, BitField bitfield, size_t numIns, const AddrShift inList[], TypeCode type, MarkCode mark)
{
#ifndef NDEBUG
    if (numIns != opcode.GetOpCodeNumIns(bitfield)) {
        std::cerr << "Invalid input list!"
                  << " op=" << opcode.Str() << " bitfield=" << bitfield
                  << " expected_num_in=" << opcode.GetOpCodeNumIns(bitfield) << " actual_num_in=" << numIns
                  << std::endl;
        UNREACHABLE();
    }
#endif
    std::vector<Gate *> inPtrList(numIns);
    auto gateSpace = this->AllocateGateSpace(numIns);
    for (size_t idx = 0; idx < numIns; idx++) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        inPtrList[idx] = (inList[idx] == Circuit::NullGate()) ? nullptr : this->LoadGatePtr(inList[idx]);
    }
    auto newGate = new (gateSpace) Gate(this->gateCounter, opcode, bitfield, inPtrList.data(), type, mark);
    this->gateCounter++;
    return this->SaveGatePtr(newGate);
}

AddrShift Circuit::NewGate(
    OpCode opcode, BitField bitfield, const std::vector<AddrShift> &inList, TypeCode type, MarkCode mark)
{
    return this->NewGate(opcode, bitfield, inList.size(), inList.data(), type, mark);
}

void Circuit::PrintAllGates() const
{
    const auto &gateList = this->GetAllGates();
    for (auto &gate : gateList) {
        this->LoadGatePtrConst(gate)->Print();
    }
}

std::vector<AddrShift> Circuit::GetAllGates() const
{
    std::vector<AddrShift> gateList;
    gateList.push_back(0);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    for (size_t out = sizeof(Gate); out < this->circuitSize;
         out += Gate::GetGateSize(
             // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
             reinterpret_cast<const Out *>(this->LoadGatePtrConst(AddrShift(out)))->GetIndex() + 1)) {
        gateList.push_back(
            this->SaveGatePtr(reinterpret_cast<const Out *>(this->LoadGatePtrConst(AddrShift(out)))->GetGateConst()));
    }
    return gateList;
}

AddrShift Circuit::SaveGatePtr(const Gate *gate) const
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return static_cast<AddrShift>(reinterpret_cast<const uint8_t *>(gate) - this->GetDataPtrConst(0));
}

Gate *Circuit::LoadGatePtr(AddrShift shift)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return reinterpret_cast<Gate *>(this->GetDataPtr(shift));
}

const Gate *Circuit::LoadGatePtrConst(AddrShift shift) const
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return reinterpret_cast<const Gate *>(this->GetDataPtrConst(shift));
}

AddrShift Circuit::GetCircuitRoot(OpCode opcode)
{
    switch (opcode) {
        case OpCode::CIRCUIT_ROOT:
            return sizeof(In) * 0 + sizeof(Out) * 0 + sizeof(Gate) * 0;  // 0 0 0: offset of circuit root
        case OpCode::STATE_ENTRY:
            return sizeof(In) * 0 + sizeof(Out) * 1 + sizeof(Gate) * 1;  // 0 1 1: offset of state entry
        case OpCode::DEPEND_ENTRY:
            return sizeof(In) * 1 + sizeof(Out) * 2 + sizeof(Gate) * 2;  // 1 2 2: offset of depend entry
        case OpCode::FRAMESTATE_ENTRY:
            return sizeof(In) * 2 + sizeof(Out) * 3 + sizeof(Gate) * 3;  // 2 3 3: offset of framestate entry
        case OpCode::RETURN_LIST:
            return sizeof(In) * 3 + sizeof(Out) * 4 + sizeof(Gate) * 4;  // 3 4 4: offset of return list
        case OpCode::THROW_LIST:
            return sizeof(In) * 4 + sizeof(Out) * 5 + sizeof(Gate) * 5;  // 4 5 5: offset of throw list
        case OpCode::CONSTANT_LIST:
            return sizeof(In) * 5 + sizeof(Out) * 6 + sizeof(Gate) * 6;  // 5 6 6: offset of constant list
        case OpCode::ALLOCA_LIST:
            return sizeof(In) * 6 + sizeof(Out) * 7 + sizeof(Gate) * 7;  // 6 7 7: offset of alloca list
        case OpCode::ARG_LIST:
            return sizeof(In) * 7 + sizeof(Out) * 8 + sizeof(Gate) * 8;  // 7 8 8: offset of arg list
        default:
            UNREACHABLE();
    }
}

Circuit::~Circuit() {}

void Circuit::AdvanceTime() const
{
    auto &time = const_cast<TimeStamp &>(this->time);
    time++;
    if (time == 0) {
        time = 1;
        this->ResetAllGateTimeStamps();
    }
}

void Circuit::ResetAllGateTimeStamps() const
{
    const auto &gateList = this->GetAllGates();
    for (auto &gate : gateList) {
        const_cast<Gate *>(this->LoadGatePtrConst(gate))->SetMark(MarkCode::EMPTY, 0);
    }
}

TimeStamp Circuit::GetTime() const
{
    return this->time;
}

MarkCode Circuit::GetMark(AddrShift gate) const
{
    return this->LoadGatePtrConst(gate)->GetMark(this->GetTime());
}

void Circuit::SetMark(AddrShift gate, MarkCode mark) const
{
    const_cast<Gate *>(this->LoadGatePtrConst(gate))->SetMark(mark, this->GetTime());
}

bool Circuit::Verify(AddrShift gate) const
{
    return this->LoadGatePtrConst(gate)->Verify();
}

AddrShift Circuit::NullGate()
{
    return -1;
}

bool Circuit::IsLoopHead(AddrShift gate) const
{
    if (gate != NullGate()) {
        const Gate *curGate = this->LoadGatePtrConst(gate);
        return curGate->GetOpCode() == OpCode::LOOP_BEGIN;
    }
    return false;
}

bool Circuit::IsSelector(AddrShift gate) const
{
    if (gate != NullGate()) {
        const Gate *curGate = this->LoadGatePtrConst(gate);
        return curGate->GetOpCode() >= OpCode::VALUE_SELECTOR_JS &&
            curGate->GetOpCode() <= OpCode::VALUE_SELECTOR_FLOAT64;
    }
    return false;
}

std::vector<AddrShift> Circuit::GetInVector(AddrShift gate) const
{
    std::vector<AddrShift> result;
    const Gate *curGate = this->LoadGatePtrConst(gate);
    for (size_t idx = 0; idx < curGate->GetNumIns(); idx++) {
        result.push_back(this->SaveGatePtr(curGate->GetInGateConst(idx)));
    }
    return result;
}

AddrShift Circuit::GetIn(AddrShift gate, size_t idx) const
{
    const Gate *curGate = this->LoadGatePtrConst(gate);
    return this->SaveGatePtr(curGate->GetInGateConst(idx));
}

bool Circuit::IsInGateNull(AddrShift gate, size_t idx) const
{
    const Gate *curGate = this->LoadGatePtrConst(gate);
    return curGate->GetInConst(idx)->IsGateNull();
}

bool Circuit::IsFirstOutNull(AddrShift gate) const
{
    const Gate *curGate = this->LoadGatePtrConst(gate);
    return curGate->IsFirstOutNull();
}

std::vector<AddrShift> Circuit::GetOutVector(AddrShift gate) const
{
    std::vector<AddrShift> result;
    const Gate *curGate = this->LoadGatePtrConst(gate);
    if (!curGate->IsFirstOutNull()) {
        const Out *curOut = curGate->GetFirstOutConst();
        result.push_back(this->SaveGatePtr(curOut->GetGateConst()));
        while (!curOut->IsNextOutNull()) {
            curOut = curOut->GetNextOutConst();
            result.push_back(this->SaveGatePtr(curOut->GetGateConst()));
        }
    }
    return result;
}

void Circuit::NewIn(AddrShift gate, size_t idx, AddrShift in)
{
    this->LoadGatePtr(gate)->NewIn(idx, this->LoadGatePtr(in));
}

void Circuit::ModifyIn(AddrShift gate, size_t idx, AddrShift in)
{
    this->LoadGatePtr(gate)->ModifyIn(idx, this->LoadGatePtr(in));
}

void Circuit::DeleteIn(AddrShift gate, size_t idx)
{
    this->LoadGatePtr(gate)->DeleteIn(idx);
}

void Circuit::DeleteGate(AddrShift gate)
{
    this->LoadGatePtr(gate)->DeleteGate();
}

void Circuit::SetOpCode(AddrShift gate, OpCode opcode)
{
    this->LoadGatePtr(gate)->SetOpCode(opcode);
}

OpCode Circuit::GetOpCode(AddrShift gate) const
{
    return this->LoadGatePtrConst(gate)->GetOpCode();
}

GateId Circuit::GetId(AddrShift gate) const
{
    return this->LoadGatePtrConst(gate)->GetId();
}

BitField Circuit::GetBitField(AddrShift gate) const
{
    return this->LoadGatePtrConst(gate)->GetBitField();
}

void Circuit::Print(AddrShift gate) const
{
    this->LoadGatePtrConst(gate)->Print();
}

std::vector<uint8_t> Circuit::GetDataSection() const
{
    return this->dataSection;
}

void Circuit::SetDataSection(const std::vector<uint8_t> &data)
{
    this->dataSection = data;
}

size_t Circuit::GetCircuitDataSize() const
{
    return this->circuitSize;
}

const void *Circuit::GetSpaceDataStartPtrConst() const
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return this->GetDataPtrConst(0);
}

const void *Circuit::GetSpaceDataEndPtrConst() const
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return this->GetDataPtrConst(this->circuitSize);
}

const uint8_t *Circuit::GetDataPtrConst(size_t offset) const
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return this->space.data() + offset;
}

uint8_t *Circuit::GetDataPtr(size_t offset)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return this->space.data() + offset;
}

size_t Circuit::GetSpaceDataSize() const
{
    return this->space.size();
}

void Circuit::SetSpaceDataSize(size_t sz)
{
    return this->space.resize(sz);
}

panda::ecmascript::FrameType Circuit::GetFrameType() const
{
    return this->frameType;
}

void Circuit::SetFrameType(panda::ecmascript::FrameType type)
{
    this->frameType = type;
}
}  // namespace kungfu
