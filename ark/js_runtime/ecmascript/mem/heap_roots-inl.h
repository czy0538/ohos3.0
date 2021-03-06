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

#ifndef ECMASCRIPT_MEM_HEAP_ROOTS_INL_H
#define ECMASCRIPT_MEM_HEAP_ROOTS_INL_H

#include <cstdint>
#include "ecmascript/class_linker/program_object.h"
#include "ecmascript/ecma_module.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/ic/ic_handler.h"
#include "ecmascript/ic/proto_change_details.h"
#include "ecmascript/jobs/micro_job_queue.h"
#include "ecmascript/jobs/pending_job.h"
#include "ecmascript/js_arguments.h"
#include "ecmascript/js_array.h"
#include "ecmascript/js_array_iterator.h"
#include "ecmascript/js_arraybuffer.h"
#include "ecmascript/js_async_function.h"
#include "ecmascript/js_dataview.h"
#include "ecmascript/js_date.h"
#include "ecmascript/js_date_time_format.h"
#include "ecmascript/js_for_in_iterator.h"
#include "ecmascript/js_function.h"
#include "ecmascript/js_generator_object.h"
#include "ecmascript/js_hclass.h"
#include "ecmascript/js_intl.h"
#include "ecmascript/js_locale.h"
#include "ecmascript/js_map.h"
#include "ecmascript/js_map_iterator.h"
#include "ecmascript/js_native_object.h"
#include "ecmascript/js_number_format.h"
#include "ecmascript/js_object-inl.h"
#include "ecmascript/js_primitive_ref.h"
#include "ecmascript/js_promise.h"
#include "ecmascript/js_regexp.h"
#include "ecmascript/js_relative_time_format.h"
#include "ecmascript/js_set.h"
#include "ecmascript/js_set_iterator.h"
#include "ecmascript/js_string_iterator.h"
#include "ecmascript/js_typed_array.h"
#include "ecmascript/js_weak_container.h"
#include "ecmascript/mem/heap_roots.h"
#include "ecmascript/mem/mem.h"

namespace panda::ecmascript {
void HeapRootManager::VisitVMRoots(const RootVisitor &visitor, const RootRangeVisitor &rangeVisitor) const
{
    ecmaVm_->Iterate(visitor);
    ecmaVm_->GetJSThread()->Iterate(visitor, rangeVisitor);
}

template<GCType gc_type>
// NOLINTNEXTLINE(readability-function-size)
void HeapRootManager::MarkObjectBody(TaggedObject *object, JSHClass *klass, const EcmaObjectRangeVisitor &visitor)
{
    // handle body
    JSType type = klass->GetObjectType();
    switch (type) {
        case JSType::JS_OBJECT:
        case JSType::JS_ERROR:
        case JSType::JS_EVAL_ERROR:
        case JSType::JS_RANGE_ERROR:
        case JSType::JS_REFERENCE_ERROR:
        case JSType::JS_TYPE_ERROR:
        case JSType::JS_URI_ERROR:
        case JSType::JS_SYNTAX_ERROR:
        case JSType::JS_ITERATOR:
            JSObject::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_GLOBAL_OBJECT:
            JSGlobalObject::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_FUNCTION_BASE:
            JSFunctionBase::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_FUNCTION:
            JSFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_GENERATOR_FUNCTION:
            JSGeneratorFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_PROXY_REVOC_FUNCTION:
            JSProxyRevocFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_PROMISE_REACTIONS_FUNCTION:
            JSPromiseReactionsFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_PROMISE_EXECUTOR_FUNCTION:
            JSPromiseExecutorFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_PROMISE_ALL_RESOLVE_ELEMENT_FUNCTION:
            JSPromiseAllResolveElementFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_ASYNC_FUNCTION:
            JSAsyncFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_ASYNC_AWAIT_STATUS_FUNCTION:
            JSAsyncAwaitStatusFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_REG_EXP:
            JSRegExp::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_SET:
            JSSet::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_MAP:
            JSMap::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_WEAK_MAP:
            JSWeakMap::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_WEAK_SET:
            JSWeakSet::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_DATE:
            JSDate::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_FORIN_ITERATOR:
            JSForInIterator::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_MAP_ITERATOR:
            JSMapIterator::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_SET_ITERATOR:
            JSSetIterator::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_ARRAY_ITERATOR:
            JSArrayIterator::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_STRING_ITERATOR:
            JSStringIterator::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_ARRAY_BUFFER:
            JSArrayBuffer::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_PROMISE:
            JSPromise::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_DATA_VIEW:
            JSDataView::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_BOUND_FUNCTION:
            JSBoundFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_ARGUMENTS:
            JSArguments::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_GENERATOR_OBJECT:
            JSGeneratorObject::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_ASYNC_FUNC_OBJECT:
            JSAsyncFuncObject::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_ARRAY:
            JSArray::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_TYPED_ARRAY:
        case JSType::JS_INT8_ARRAY:
        case JSType::JS_UINT8_ARRAY:
        case JSType::JS_UINT8_CLAMPED_ARRAY:
        case JSType::JS_INT16_ARRAY:
        case JSType::JS_UINT16_ARRAY:
        case JSType::JS_INT32_ARRAY:
        case JSType::JS_UINT32_ARRAY:
        case JSType::JS_FLOAT32_ARRAY:
        case JSType::JS_FLOAT64_ARRAY:
            JSTypedArray::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_PRIMITIVE_REF:
            JSPrimitiveRef::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_PROXY:
            JSProxy::Cast(object)->Visitor(visitor);
            break;
        case JSType::HCLASS:
            // semi gc is not needed to visit dyn class
            if (gc_type != GCType::SEMI_GC) {
                JSHClass::Cast(object)->Visitor(visitor);
            }
            break;
        case JSType::STRING:
        case JSType::JS_NATIVE_POINTER:
            break;
        case JSType::TAGGED_ARRAY:
        case JSType::TAGGED_DICTIONARY:
            TaggedArray::Cast(object)->Visitor(visitor);
            break;
        case JSType::GLOBAL_ENV:
            GlobalEnv::Cast(object)->Visitor(visitor);
            break;
        case JSType::ACCESSOR_DATA:
        case JSType::INTERNAL_ACCESSOR:
            AccessorData::Cast(object)->Visitor(visitor);
            break;
        case JSType::SYMBOL:
            JSSymbol::Cast(object)->Visitor(visitor);
            break;
        case JSType::OBJECT_WRAPPER:
            ObjectWrapper::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_GENERATOR_CONTEXT:
            GeneratorContext::Cast(object)->Visitor(visitor);
            break;
        case JSType::PROTOTYPE_HANDLER:
            PrototypeHandler::Cast(object)->Visitor(visitor);
            break;
        case JSType::TRANSITION_HANDLER:
            TransitionHandler::Cast(object)->Visitor(visitor);
            break;
        case JSType::PROPERTY_BOX:
            PropertyBox::Cast(object)->Visitor(visitor);
            break;
        case JSType::PROTO_CHANGE_MARKER:
            break;
        case JSType::PROTOTYPE_INFO:
            ProtoChangeDetails::Cast(object)->Visitor(visitor);
            break;
        case JSType::TEMPLATE_MAP:
            UNREACHABLE();
            break;
        case JSType::PROMISE_CAPABILITY:
            PromiseCapability::Cast(object)->Visitor(visitor);
            break;
        case JSType::PROMISE_RECORD:
            PromiseRecord::Cast(object)->Visitor(visitor);
            break;
        case JSType::RESOLVING_FUNCTIONS_RECORD:
            ResolvingFunctionsRecord::Cast(object)->Visitor(visitor);
            break;
        case JSType::PROMISE_REACTIONS:
            PromiseReaction::Cast(object)->Visitor(visitor);
            break;
        case JSType::PROMISE_ITERATOR_RECORD:
            PromiseIteratorRecord::Cast(object)->Visitor(visitor);
            break;
        case JSType::MICRO_JOB_QUEUE:
            job::MicroJobQueue::Cast(object)->Visitor(visitor);
            break;
        case JSType::PENDING_JOB:
            job::PendingJob::Cast(object)->Visitor(visitor);
            break;
        case JSType::FUNCTION_EXTRA_INFO:
            JSFunctionExtraInfo::Cast(object)->Visitor(visitor);
            break;
        case JSType::COMPLETION_RECORD:
            CompletionRecord::Cast(object)->Visitor(visitor);
            break;
        case JSType::ECMA_MODULE:
            EcmaModule::Cast(object)->Visitor(visitor);
            break;
        case JSType::PROGRAM:
            Program::Cast(object)->Visitor(visitor);
            break;
        case JSType::LEXICAL_FUNCTION:
            LexicalFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_INTL:
            JSIntl::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_NUMBER_FORMAT:
            JSNumberFormat::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_LOCALE:
            JSLocale::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_DATE_TIME_FORMAT:
            JSDateTimeFormat::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_RELATIVE_TIME_FORMAT:
            JSRelativeTimeFormat::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_INTL_BOUND_FUNCTION:
            JSIntlBoundFunction::Cast(object)->Visitor(visitor);
            break;
        case JSType::JS_NATIVE_OBJECT:
            JSNativeObject::Cast(object)->Visitor(visitor);
            break;
        default:
            UNREACHABLE();
    }
}
}  // namespace panda::ecmascript

#endif  // ECMASCRIPT_MEM_HEAP_ROOTS_INL_H
