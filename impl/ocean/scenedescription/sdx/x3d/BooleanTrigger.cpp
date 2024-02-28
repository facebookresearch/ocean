// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdx/x3d/BooleanTrigger.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

BooleanTrigger::BooleanTrigger(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

BooleanTrigger::NodeSpecification BooleanTrigger::specifyNode()
{
	NodeSpecification specification("BooleanTrigger");

	registerField(specification, "set_triggerTime", setTriggerTime_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "triggerTrue", triggerTrue_, ACCESS_GET);

	X3DChildNode::registerFields(specification);

	return specification;
}

bool BooleanTrigger::onFieldChanging(const std::string& fieldName, const Field& field)
{
	if (fieldName == "set_triggerTime")
	{
		triggerTrue_.setValue(true, field.timestamp());
		forwardThatFieldHasBeenChanged("triggerTrue", triggerTrue_);

		return true;
	}

	return X3DChildNode::onFieldChanging(fieldName, field);
}

size_t BooleanTrigger::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
