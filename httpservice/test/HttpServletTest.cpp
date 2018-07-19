/*=============================================================================

Library: CppMicroServices

Copyright (c) The CppMicroServices developers. See the COPYRIGHT
file at the top-level directory of this distribution and at
https://github.com/CppMicroServices/CppMicroServices/COPYRIGHT .

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/

#include "TestingConfig.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>

#include "TestEnvironment.h"
#include "TestHttpRequests.h"
#include "cppmicroservices/httpservice/HttpServletPart.h"
#include "cppmicroservices/util/FileSystem.h"

namespace us = cppmicroservices;

TEST(HttpServletTest, GetParts_oneFileMultipart)
{

  TestEnvironment testEnv("8180");

  testEnv.HttpSend(Queries::POST_MULTIPART_ONE_FILE_REQUEST);

  TestServlet* servlet = dynamic_cast<TestServlet*>(testEnv.GetServlet().get());

  ASSERT_TRUE((nullptr != servlet)) << "Servlet must be created";

  us::HttpServletRequest* request = servlet->GetProcessedRequest();

  ASSERT_TRUE(servlet->OperationReceived())
    << "POST operation must have hit the servlet";
  ASSERT_TRUE((nullptr != request)) << "Processed request must not be null";

  size_t ONE = 1;
  ASSERT_EQ(ONE, request->GetParts().size())
    << "One file is transmitted in this request";
}

TEST(HttpServletTest, GetPart_oneFileMultipart)
{
  TestEnvironment testEnv("8181");

  testEnv.HttpSend(Queries::POST_MULTIPART_ONE_FILE_REQUEST);

  TestServlet* servlet = dynamic_cast<TestServlet*>(testEnv.GetServlet().get());

  ASSERT_TRUE((nullptr != servlet)) << "Servlet must be created";

  us::HttpServletRequest* request = servlet->GetProcessedRequest();

  ASSERT_TRUE(servlet->OperationReceived())
    << "POST operation must have hit the servlet";
  ASSERT_TRUE((nullptr != request)) << "Processed request must not be null";

  ASSERT_TRUE((nullptr != request->GetPart("sampleFileFormName")))
    << "The part is accessible through GetPart";
}

TEST(HttpServletTest, GetAttribute_GET_with_plus_as_space)
{
  TestEnvironment testEnv("8182");

  testEnv.HttpSend(Queries::GET_PARAMETERS_WITH_PLUS_AS_SPACE);

  TestServlet* servlet = dynamic_cast<TestServlet*>(testEnv.GetServlet().get());

  ASSERT_TRUE((nullptr != servlet)) << "Servlet must be created";

  auto request = servlet->GetProcessedRequest();

  ASSERT_TRUE(servlet->OperationReceived())
    << "GET operation must have hit the servlet";
  ASSERT_TRUE((nullptr != request)) << "Processed request must not be null";

  us::Any parameter = request->GetParameter("a");
  ASSERT_FALSE(parameter.Empty()) << "Parameter 'a' must be found";
  ASSERT_EQ("value+of+a", parameter.ToString())
    << "The value of A must be 'value+of+a'";
}

TEST(HttpServletTest, GetAttribute_GET)
{
  TestEnvironment testEnv("8183");

  testEnv.HttpSend(Queries::GET_PARAMETERS_SIMPLE);

  TestServlet* servlet = dynamic_cast<TestServlet*>(testEnv.GetServlet().get());

  ASSERT_TRUE((nullptr != servlet)) << "Servlet must be created";

  auto request = servlet->GetProcessedRequest();

  ASSERT_TRUE(servlet->OperationReceived())
    << "GET operation must have hit the servlet";
  ASSERT_TRUE((nullptr != request)) << "Processed request must not be null";

  us::Any parameter = request->GetParameter("a");
  ASSERT_FALSE(parameter.Empty()) << "Parameter 'a' must be found";
  ASSERT_EQ("value of a", parameter.ToString())
    << "The value of A must be 'value of a'";
}

TEST(HttpServletTest, GetAttribute_GET_empty_parameter)
{
  TestEnvironment testEnv("8184");

  testEnv.HttpSend(Queries::GET_PARAMETERS_SIMPLE);

  TestServlet* servlet = dynamic_cast<TestServlet*>(testEnv.GetServlet().get());

  ASSERT_TRUE((nullptr != servlet)) << "Servlet must be created";

  auto request = servlet->GetProcessedRequest();

  ASSERT_TRUE(servlet->OperationReceived())
    << "GET operation must have hit the servlet";
  ASSERT_TRUE((nullptr != request)) << "Processed request must not be null";

  us::Any parameter = request->GetParameter("iDoNoExist");
  ASSERT_TRUE(parameter.Empty()) << "Parameter 'iDoNoExist' must not be found";
}

TEST(HttpServletTest, GetAttribute_POST_form_data)
{
  TestEnvironment testEnv("8184");

  testEnv.HttpSend(Queries::POST_FORM_DATA);
  std::string EXPECTED_VALUE = "the A value";

  TestServlet* servlet = dynamic_cast<TestServlet*>(testEnv.GetServlet().get());

  ASSERT_TRUE((nullptr != servlet)) << "Servlet must be created";

  auto request = servlet->GetProcessedRequest();

  ASSERT_TRUE(servlet->OperationReceived())
    << "GET operation must have hit the servlet";
  ASSERT_TRUE((nullptr != request)) << "Processed request must not be null";

  cppmicroservices::HttpServletPart* part;
  size_t partCount = request->GetParts().size();
  size_t TWO = 2;
  ASSERT_EQ(TWO, partCount) << "We must have two parts in this request";

  try {
    part = request->GetPart("A");
  } catch (const std::exception& e) {
    FAIL() << "Unable to retrieve part 'A' (" << e.what() << ")";
  }
  std::istream* partValue = part->GetInputStream();
  std::string value{ std::istreambuf_iterator<char>(*partValue),
                     std::istreambuf_iterator<char>() };

  delete partValue;
  ASSERT_EQ(EXPECTED_VALUE, value)
    << "The part value must be '" << EXPECTED_VALUE << "'";

  us::Any parameter = request->GetParameter("A");
  ASSERT_FALSE(parameter.Empty()) << "Parameter 'A' must be found";
  ASSERT_EQ(EXPECTED_VALUE, parameter.ToString())
    << "Parameter 'A' must be '" << EXPECTED_VALUE << "'";
}
