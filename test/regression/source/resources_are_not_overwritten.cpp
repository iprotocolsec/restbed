/*
 * Copyright (c) 2013, 2014, 2015 Corvusoft
 *
 * bug tracker issue #3
 */

//System Includes
#include <memory>

//Project Includes
#include <restbed>

//External Includes
#include <catch.hpp>
#include <corvusoft/framework/http>

//System Namespaces
using std::shared_ptr;
using std::make_shared;

//Project Namespaces
using namespace restbed;

//External Namespaces
using namespace framework;

shared_ptr< Service > m_service;

Response json_get_handler( const Request& )
{
    Response response;
    response.set_status_code( 200 );
    response.set_body( "{ name: \"value\" }" );
    
    return response;
}

Response xml_get_handler( const Request& )
{
    Response response;
    response.set_status_code( 401 );
    response.set_body( "<name>value</value>" );
    
    return response;
}

TEST_CASE( "overwrite existing resource", "[resource]" )
{
    auto initial_resource = make_shared< Resource >( );
    initial_resource->set_path( "TestResource" );
    initial_resource->set_method_handler( "GET", &json_get_handler );

    auto secondary_resource = make_shared< Resource >( );
    secondary_resource->set_path( "TestResource" );
    secondary_resource->set_method_handler( "GET", &xml_get_handler );
    
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );
    
    m_service = make_shared< Service >( settings );
    m_service->publish( initial_resource );
    m_service->publish( secondary_resource );
    m_service->start( );

    Http::Request request;
    request.method = "GET";
    request.port = 1984;
    request.host = "localhost";
    request.path = "/TestResource";

    auto response = Http::get( request );
    
    REQUIRE( 401 == response.status_code );
    
    m_service->stop( );
}

TEST_CASE( "add alternative resource", "[resource]" )
{
    auto initial_resource = make_shared< Resource >( );
    initial_resource->set_path( "TestResource" );
    initial_resource->set_header_filter( "Content-Type", "application/json" );
    initial_resource->set_method_handler( "GET", &json_get_handler );
    
    auto secondary_resource = make_shared< Resource >( );
    secondary_resource->set_path( "TestResource" );
    secondary_resource->set_header_filter( "Content-Type", "application/xml" );
    secondary_resource->set_method_handler( "GET", &xml_get_handler );
    
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );
    
    m_service = make_shared< Service >( settings );
    m_service->publish( initial_resource );
    m_service->publish( secondary_resource );
    m_service->start( );

    Http::Request request;
    request.method = "GET";
    request.port = 1984;
    request.host = "localhost";
    request.path = "/TestResource";
    request.headers = { { "Content-Type", "application/xml" } };

    auto response = Http::get( request );
    
    REQUIRE( 401 == response.status_code );
    
    m_service->stop( );
}
