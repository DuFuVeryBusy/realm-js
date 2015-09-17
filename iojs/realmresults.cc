#include <node.h>

#include "realmresults.h"
#include "realmutils.hpp"

using namespace v8;
using namespace realm;

Persistent<Function> RealmResults::constructor;

RealmResults::RealmResults(realm::Results results) : m_results(std::move(results)) {
}

RealmResults::~RealmResults() {}

void RealmResults::Init(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, RealmResults::New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "RealmResults"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Methods
    NODE_SET_PROTOTYPE_METHOD(tpl, "sortByProperty",     RealmResults::SortByProperty);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "RealmResults"), tpl->GetFunction());
}


void RealmResults::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new RealmResults(...)`
        args.GetReturnValue().Set(args.This());
    } else {
        // TODO: Invoked as plain function `RealmResults(...)`, turn into construct call.
    }

}

void RealmResults::SortByProperty(const v8::FunctionCallbackInfo<v8::Value>& args) {
    RealmResults *results = ObjectWrap::Unwrap<RealmResults>(args.This());

    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (!ValidateArgumentRange(args.Length(), 1, 2)) {
        return;
    }

    std::string propName = ToString(args[0]);
    Property *prop = results->m_results.object_schema.property_for_name(propName);
    if (!prop) {
        makeError(isolate, "Property '" + propName + "' does not exist on object type '" + 
            results->m_results.object_schema.name + "'");
        return;
    }

    bool ascending = true;
    if (args.Length() == 2) {
        ascending = *args[1]->ToBoolean();
    }

    SortOrder sort = {{prop->table_column}, {ascending}};
    results->m_results.setSort(sort);
}
