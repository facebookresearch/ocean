// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdx/x3d/TimeTrigger.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

TimeTrigger::TimeTrigger(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

TimeTrigger::NodeSpecification TimeTrigger::specifyNode()
{
	NodeSpecification specification("TimeTrigger");

	registerField(specification, "set_boolean", setBoolean_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "triggerTime", triggerTime_, ACCESS_GET);

	X3DChildNode::registerFields(specification);

	return specification;
}

bool TimeTrigger::onFieldChanging(const std::string& fieldName, const Field& field)
{
	if (fieldName == "set_boolean")
	{
		triggerTime_.setValue(field.timestamp(), field.timestamp());
		forwardThatFieldHasBeenChanged("triggerTime", triggerTime_);

		return true;
	}

	return X3DChildNode::onFieldChanging(fieldName, field);
}

size_t TimeTrigger::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
